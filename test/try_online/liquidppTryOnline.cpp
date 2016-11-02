// Copyright 2010 Dean Michael Berris.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <memory>
#include <thread>
#include <boost/network/protocol/http/server.hpp>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cassert>

#include <boost/property_tree/json_parser.hpp>

#include <liquidpp.hpp>
#include "../../benchmark/ExampleData.hpp"

namespace http = boost::network::http;
namespace utils = boost::network::utils;

struct LiquidppTryServer;
typedef http::server<LiquidppTryServer> server;

class LiquidppRenderer
{
private:
   liquidpp::Context c;
   
public:
   LiquidppRenderer()
    : c(liquidpp::example_data::initLiquidContext())
   {}
   
   std::string operator()(liquidpp::string_view templ) const
   {
      return liquidpp::render(templ, c);
   }
};

struct FileCache {
  typedef std::map<std::string, std::pair<void *, std::size_t> > region_map;
  typedef std::map<std::string, std::vector<server::response_header> > meta_map;

  std::string doc_root_;
  region_map regions;
  meta_map file_headers;
  std::mutex cache_mutex;

  explicit FileCache(std::string doc_root) : doc_root_(std::move(doc_root)) {}

  ~FileCache() throw() {
    for (auto &region : regions) {
      munmap(region.second.first, region.second.second);
    }
  }

  bool has(std::string const &path) {
    std::unique_lock<std::mutex> lock(cache_mutex);
    return regions.find(doc_root_ + path) != regions.end();
  }
  
  static const char* mimeType(std::string const &path) {
     std::string ext;
     auto pos = path.find_last_of(".");
     if (pos != std::string::npos)
        ext = path.substr(pos+1);
     
     if (ext == "html" || ext == "htm")
        return "text/html";
     if (ext == "js")
        return "text/javascript";
     if (ext == "css")
        return "text/css";
     
     return "x-application/octet-stream";
  }

  bool add(std::string const &path) {
    std::unique_lock<std::mutex> lock(cache_mutex);
    std::string real_filename = doc_root_ + path;
    if (regions.find(real_filename) != regions.end()) return true;
#ifdef O_NOATIME
    int fd = open(real_filename.c_str(), O_RDONLY | O_NOATIME | O_NONBLOCK);
#else
    int fd = open(real_filename.c_str(), O_RDONLY | O_NONBLOCK);
#endif
    if (fd == -1) return false;
    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
      close(fd);
      return false;
    }
    void *region = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (region == MAP_FAILED) {
      close(fd);
      return false;
    }

    regions.insert(std::make_pair(real_filename, std::make_pair(region, size)));
    static server::response_header common_headers[] = {
        {"Connection", "close"},
        {"Content-Type", mimeType(path)},
        {"Content-Length", "0"}};
    std::vector<server::response_header> headers(common_headers,
                                                 common_headers + 3);
    headers[2].value = std::to_string(size);
    file_headers.insert(std::make_pair(real_filename, headers));
    close(fd);
    return true;
  }

  std::pair<void *, std::size_t> get(std::string const &path) {
    std::unique_lock<std::mutex> lock(cache_mutex);
    region_map::const_iterator region = regions.find(doc_root_ + path);
    if (region != regions.end())
      return region->second;
    else
      return std::pair<void *, std::size_t>(0, 0);
  }

  boost::iterator_range<std::vector<server::response_header>::iterator> meta(
      std::string const &path) {
    std::unique_lock<std::mutex> lock(cache_mutex);
    static std::vector<server::response_header> empty_vector;
    auto headers = file_headers.find(doc_root_ + path);
    if (headers != file_headers.end()) {
      auto begin = headers->second.begin(), end = headers->second.end();
      return boost::make_iterator_range(begin, end);
    } else
      return boost::make_iterator_range(empty_vector);
  }
};

boost::property_tree::ptree errorToPropertyTree(liquidpp::Exception& e)
{
   boost::property_tree::ptree err;
   err.add("message", e.what());
   err.add("line", e.position().line);
   err.add("column", e.position().column);
   return err;
}

boost::property_tree::ptree errorToPropertyTree(std::exception& e)
{
   boost::property_tree::ptree err;
   err.add("message", e.what());
   return err;
}

struct ConnectionHandler : std::enable_shared_from_this<ConnectionHandler> {
  explicit ConnectionHandler(FileCache &cache, const LiquidppRenderer& renderer) : mFileCache(cache), mRenderer(renderer) {}

  void operator()(std::string const &path, server::connection_ptr connection,
                  bool serve_body, liquidpp::string_view requestBody = liquidpp::string_view{}) {
    bool ok = mFileCache.has(path);
    if (!ok) ok = mFileCache.add(path);
    if (ok) {
      send_headers(mFileCache.meta(path), connection);
      if (serve_body)
                sendFile(mFileCache.get(path), 0, connection);
      return;
    } else if (path == "/render" || path == "render") {
      if (!requestBody.empty())
      {
         boost::property_tree::ptree err;
            
         try {
            auto rendered = mRenderer(requestBody);
            static server::response_header headers[] = {{"Connection", "close"},
                                                         {"Content-Type", "text/plain"}};
            connection->set_status(server::connection::ok);
            connection->set_headers(boost::make_iterator_range(headers, headers + 2));
            connection->write(rendered);
            return;
         } catch(liquidpp::Exception& e) {
            err = errorToPropertyTree(e);
         } catch(std::exception& e) {
            err = errorToPropertyTree(e);
         }
         
         std::ostringstream oss;
         boost::property_tree::write_json(oss, err, true);

         static server::response_header headers[] = {{"Connection", "close"},
                                                      {"Content-Type", "application/json"}};
         connection->set_status(server::connection::bad_request);
         connection->set_headers(boost::make_iterator_range(headers, headers + 2));
         connection->write(oss.str());
         return;
      }
    }

    notFound(path, connection);
  }

  void notFound(std::string const &, server::connection_ptr connection) {
    static server::response_header headers[] = {{"Connection", "close"},
                                                {"Content-Type", "text/plain"}};
    connection->set_status(server::connection::not_found);
    connection->set_headers(boost::make_iterator_range(headers, headers + 2));
    connection->write("File Not Found!");
  }

  template <class Range>
  void send_headers(Range const &headers, server::connection_ptr connection) {
    connection->set_status(server::connection::ok);
    connection->set_headers(headers);
  }

  void sendFile(std::pair<void *, std::size_t> mmaped_region, off_t offset,
                 server::connection_ptr connection) {
    // chunk it up page by page
    std::size_t adjusted_offset = offset + 4096;
    off_t rightmost_bound = std::min(mmaped_region.second, adjusted_offset);
    auto self = this->shared_from_this();
    connection->write(
        boost::asio::const_buffers_1(
            static_cast<char const *>(mmaped_region.first) + offset,
            rightmost_bound - offset),
        [=](boost::system::error_code const &ec) {
          self->handle_chunk(mmaped_region, rightmost_bound, connection, ec);
        });
  }

  void handle_chunk(std::pair<void *, std::size_t> mmaped_region, off_t offset,
                    server::connection_ptr connection,
                    boost::system::error_code const &ec) {
    assert(offset >= 0);
    if (!ec && static_cast<std::size_t>(offset) < mmaped_region.second)
            sendFile(mmaped_region, offset, connection);
  }

  FileCache &mFileCache;
  const LiquidppRenderer& mRenderer;
};

struct InputConsumer : public std::enable_shared_from_this<InputConsumer> {
  // Maximum size for incoming request bodies.
  static constexpr std::size_t MAX_INPUT_BODY_SIZE = 2 << 16;

  explicit InputConsumer(std::shared_ptr<ConnectionHandler> h,
                          server::request r)
      : mRequest(std::move(r)), mHandler(std::move(h)), mContentLength {0} {
    for (const auto &header : mRequest.headers) {
      if (boost::iequals(header.name, "content-length")) {
                mContentLength = std::stoul(header.value);
        std::cerr << "Content length: " << mContentLength << '\n';
        break;
      }
    }
  }

  void operator()(server::connection::input_range input, boost::system::error_code ec,
                  std::size_t bytes_transferred,
                  server::connection_ptr connection) {
    std::cerr << "Callback: " << bytes_transferred << "; ec = " << ec << '\n';
    if (ec == boost::asio::error::eof) return;
    if (!ec) {
      if (empty(input))
        return (*mHandler)(mRequest.destination, connection, true);
            mRequest.body.insert(mRequest.body.end(), boost::begin(input),
                           boost::end(input));
      if (mRequest.body.size() > MAX_INPUT_BODY_SIZE) {
        connection->set_status(server::connection::bad_request);
        static server::response_header error_headers[] = {
            {"Connection", "close"}};
        connection->set_headers(
            boost::make_iterator_range(error_headers, error_headers + 1));
        connection->write("Body too large.");
        return;
      }
      std::cerr << "Body: " << mRequest.body << '\n';
      if (mRequest.body.size() == mContentLength)
        return (*mHandler)(mRequest.destination, connection, true, mRequest.body);
      std::cerr << "Scheduling another read...\n";
      auto self = this->shared_from_this();
      connection->read([self](server::connection::input_range input,
                              boost::system::error_code ec, std::size_t bytes_transferred,
                              server::connection_ptr connection) {
        (*self)(input, ec, bytes_transferred, connection);
      });
    }
  }

  server::request mRequest;
  std::shared_ptr<ConnectionHandler> mHandler;
  size_t mContentLength;
};

struct LiquidppTryServer {
  explicit LiquidppTryServer(FileCache &cache, const LiquidppRenderer& renderer) : mCache(cache), mRenderer(renderer) {}

  void operator()(server::request const &request,
                  server::connection_ptr connection) {
    if (request.method == "HEAD") {
      std::shared_ptr<ConnectionHandler> h(new ConnectionHandler(mCache, mRenderer));
      (*h)(request.destination, connection, false);
    } else if (request.method == "GET") {
      std::shared_ptr<ConnectionHandler> h(new ConnectionHandler(mCache, mRenderer));
      (*h)(request.destination, connection, true);
    } else if (request.method == "PUT" || request.method == "POST") {
      auto h = std::make_shared<ConnectionHandler>(mCache, mRenderer);
      auto c = std::make_shared<InputConsumer>(h, request);
      connection->read([c](server::connection::input_range input,
                           boost::system::error_code ec, std::size_t bytes_transferred,
                           server::connection_ptr connection) {
        (*c)(input, ec, bytes_transferred, connection);
      });
    } else {
      static server::response_header error_headers[] = {
          {"Connection", "close"}};
      connection->set_status(server::connection::not_supported);
      connection->set_headers(
          boost::make_iterator_range(error_headers, error_headers + 1));
      connection->write("Method not supported.");
    }
  }

  FileCache &mCache;
  const LiquidppRenderer& mRenderer;
};

int main(int, char *[]) {
  try {
    LiquidppRenderer renderer;
        FileCache cache(".");
        LiquidppTryServer handler(cache, renderer);
    server::options options(handler);
    options.reuse_address(true);
    server instance(options.thread_pool(std::make_shared<utils::thread_pool>(4))
                        .address("0.0.0.0")
                        .port("8000"));
    instance.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
