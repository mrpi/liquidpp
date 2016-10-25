#include "catch.hpp"

#include <liquidpp.hpp>

#include <regex>

using namespace liquidpp::literals;

TEST_CASE("Filter: escape")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Have you read 'James & the Giant Peach'?" | escape }})", c);
      REQUIRE(rendered == "Have you read &#39;James &amp; the Giant Peach&#39;?");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Tetsuro Takara" | escape }})", c);
      REQUIRE(rendered == "Tetsuro Takara");
   }
}

TEST_CASE("Filter: size")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | size }})", c);
      REQUIRE(rendered == "28");
   }

   {
      std::map<std::string, std::vector<int>> m{{"pages", {0,1,2,3,4,5,6,7,8,9,10}}};
      c.set("site", m);

      auto rendered = liquidpp::render(R"({% if site.pages.size > 10 -%}
  This is a big website!
{%- endif %})", c);
      REQUIRE(rendered == "This is a big website!");
   }
}

TEST_CASE("Filter: date")
{
   liquidpp::Context c;
   c.set("article", std::map<std::string, std::string>{
      {"published_at", "2015-07-17 13:14:15"},
      {"published_at1", "2015-07-17 13:14"},
      {"published_at2", "2015-07-17"}
   });

   {
      auto rendered = liquidpp::render(R"({{ article.published_at | date: "%a, %b %d, %y" }})", c);
      REQUIRE(rendered == "Fri, Jul 17, 15");
   }

   {
      auto rendered = liquidpp::render(R"({{ article.published_at1 | date: "%a, %b %d, %y" }})", c);
      REQUIRE(rendered == "Fri, Jul 17, 15");
   }

   {
      auto rendered = liquidpp::render(R"({{ article.published_at2 | date: "%a, %b %d, %y" }})", c);
      REQUIRE(rendered == "Fri, Jul 17, 15");
   }

   {
      auto rendered = liquidpp::render(R"({{ article.published_at | date: "%Y" }})", c);
      REQUIRE(rendered == "2015");
   }

   {
      auto rendered = liquidpp::render(R"({{ "March 14, 2016" | date: "%b %d, %y" }})", c);
      REQUIRE(rendered == "Mar 14, 16");
   }

   {
      auto rendered = liquidpp::render(R"(This page was last updated at {{ "now" | date: "%Y-%m-%d %H:%M" }}.)", c);
      std::regex rx("This page was last updated at [0-9]{4}-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9].");
      if (!std::regex_match(rendered, rx))
         FAIL("Rendered did not match expression: " << rendered);
   }
}

TEST_CASE("Filter: downcase")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Parker Moore" | downcase }})", c);
      REQUIRE(rendered == "parker moore");
   }

   {
      auto rendered = liquidpp::render(R"({{ "apple" | downcase }})", c);
      REQUIRE(rendered == "apple");
   }
}

TEST_CASE("Filter: upcase")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Parker Moore" | upcase }})", c);
      REQUIRE(rendered == "PARKER MOORE");
   }

   {
      auto rendered = liquidpp::render(R"({{ "APPLE" | upcase }})", c);
      REQUIRE(rendered == "APPLE");
   }
}

TEST_CASE("Filter: capitalize")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "title" | capitalize }})", c);
      REQUIRE(rendered == "Title");
   }

   {
      auto rendered = liquidpp::render(R"({{ "my great title" | capitalize }})", c);
      REQUIRE(rendered == "My great title");
   }
}

TEST_CASE("Filter: append")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "/my/fancy/url" | append: ".html" }})", c);
      REQUIRE(rendered == "/my/fancy/url.html");
   }

   {
      auto rendered = liquidpp::render(R"({% assign filename = "/index.html" %}
{{ "website.com" | append: filename }})", c);
      auto expected = R"(
website.com/index.html)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: abs")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ -17 | abs }})", c);
      REQUIRE(rendered == "17");
   }

   {
      auto rendered = liquidpp::render(R"({{ 4 | abs }})", c);
      REQUIRE(rendered == "4");
   }

   {
      auto rendered = liquidpp::render(R"({{ "-19.86" | abs }})", c);
      REQUIRE(rendered == "19.86");
   }
}

TEST_CASE("Filter: default")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ product_price | default: 2.99 }})", c);
      REQUIRE(rendered == "2.99");
   }

   {
      auto rendered = liquidpp::render(R"({% assign product_price = 4.99 %}{{ product_price | default: 2.99 }})", c);
      REQUIRE(rendered == "4.99");
   }

   {
      auto rendered = liquidpp::render(R"({% assign product_price = "" %}{{ product_price | default: 2.99 }})", c);
      REQUIRE(rendered == "2.99");
   }
}

TEST_CASE("Filter: ceil")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 1.2 | ceil }})", c);
      REQUIRE(rendered == "2");
   }

   {
      auto rendered = liquidpp::render(R"({{ 2.0 | ceil }})", c);
      REQUIRE(rendered == "2");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | ceil }})", c);
      REQUIRE(rendered == "184");
   }

   {
      auto rendered = liquidpp::render(R"({{ "3.5" | ceil }})", c);
      REQUIRE(rendered == "4");
   }
}

TEST_CASE("Filter: floor")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 1.2 | floor }})", c);
      REQUIRE(rendered == "1");
   }

   {
      auto rendered = liquidpp::render(R"({{ 2.0 | floor }})", c);
      REQUIRE(rendered == "2");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | floor }})", c);
      REQUIRE(rendered == "183");
   }

   {
      auto rendered = liquidpp::render(R"({{ "3.5" | floor }})", c);
      REQUIRE(rendered == "3");
   }
}

TEST_CASE("Filter: divided_by")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 16 | divided_by: 4 }})", c);
      REQUIRE(rendered == "4");
   }

   {
      auto rendered = liquidpp::render(R"({{ 5 | divided_by: 3 }})", c);
      REQUIRE(rendered == "1");
   }

   {
      auto rendered = liquidpp::render(R"({{ 20 | divided_by: 7 }})", c);
      REQUIRE(rendered == "2");
   }

   {
      auto rendered = liquidpp::render(R"({{ 20 | divided_by: 7.0 }})", c);
      REQUIRE(rendered == "2.857142857142857");
   }

   {
      auto rendered = liquidpp::render(R"({% assign my_integer = 7 %}
{{ 20 | divided_by: my_integer }})", c);
      auto expected = R"(
2)";
      REQUIRE(rendered == expected);
   }

   {
      auto rendered = liquidpp::render(R"({% assign my_integer = 7 -%}
{% assign my_float = my_integer | times: 1.0 -%}
{{ 20 | divided_by: my_float }})", c);
      REQUIRE(rendered == "2.857142857142857");
   }
}

TEST_CASE("Filter: times")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 3 | times: 2 }})", c);
      REQUIRE(rendered == "6");
   }

   {
      auto rendered = liquidpp::render(R"({{ 24 | times: 7 }})", c);
      REQUIRE(rendered == "168");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | times: 12 }})", c);
      REQUIRE(rendered == "2200.284");
   }
}

TEST_CASE("Filter: escape_once")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "1 < 2 & 3" | escape_once }})", c);
      REQUIRE(rendered == "1 &lt; 2 &amp; 3");
   }

   {
      auto rendered = liquidpp::render(R"({{ "1 &lt; 2 &amp; 3" | escape_once }})", c);
      REQUIRE(rendered == "1 &lt; 2 &amp; 3");
   }
}

TEST_CASE("Filter: lstrip")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "          So much room for activities!          " | lstrip }})", c);
      REQUIRE(rendered == "So much room for activities!          ");
   }

   {
      auto rendered = liquidpp::render("{{ \" \t\t  \r\n \" | lstrip }}", c);
      REQUIRE(rendered == "");
   }
}

TEST_CASE("Filter: minus")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 4 | minus: 2 }})", c);
      REQUIRE(rendered == "2");
   }

   {
      auto rendered = liquidpp::render(R"({{ 16 | minus: 4 }})", c);
      REQUIRE(rendered == "12");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | minus: 12 }})", c);
      REQUIRE(rendered == "171.357");
   }
}

TEST_CASE("Filter: modulo")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 3 | modulo: 2 }})", c);
      REQUIRE(rendered == "1");
   }

   {
      auto rendered = liquidpp::render(R"({{ 24 | modulo: 7 }})", c);
      REQUIRE(rendered == "3");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | modulo: 12 }})", c);
      REQUIRE((rendered == "3.357" || rendered == "3.356999999999999"));
   }
}

TEST_CASE("Filter: newline_to_br")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({% capture string_with_newlines %}
Hello
there
{% endcapture %}

{{ string_with_newlines | newline_to_br }})", c);
      auto expected = R"(

<br />
Hello<br />
there<br />
)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: plus")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 4 | plus: 2 }})", c);
      REQUIRE(rendered == "6");
   }

   {
      auto rendered = liquidpp::render(R"({{ 16 | plus: 4 }})", c);
      REQUIRE(rendered == "20");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | plus: 12 }})", c);
      REQUIRE(rendered == "195.357");
   }
}

TEST_CASE("Filter: prepend")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "apples, oranges, and bananas" | prepend: "Some fruit: " }})", c);
      REQUIRE(rendered == "Some fruit: apples, oranges, and bananas");
   }

   {
      auto rendered = liquidpp::render(R"({% assign url = "liquidmarkup.com" %}

{{ "/index.html" | prepend: url }})", c);
      auto expected = R"(

liquidmarkup.com/index.html)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: remove")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "I strained to see the train through the rain" | remove: "rain" }})", c);
      REQUIRE(rendered == "I sted to see the t through the ");
   }
}

TEST_CASE("Filter: remove_first")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "I strained to see the train through the rain" | remove_first: "rain" }})", c);
      REQUIRE(rendered == "I sted to see the train through the rain");
   }
}

TEST_CASE("Filter: replace")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Take my protein pills and put my helmet on" | replace: "my", "your" }})", c);
      REQUIRE(rendered == "Take your protein pills and put your helmet on");
   }
}

TEST_CASE("Filter: replace_first")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({% assign my_string = "Take my protein pills and put my helmet on" %}
{{ my_string | replace_first: "my", "your" }})", c);
      auto expected = R"(
Take your protein pills and put my helmet on)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: round")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ 1.2 | round }})", c);
      REQUIRE(rendered == "1");
   }

   {
      auto rendered = liquidpp::render(R"({{ 2.7 | round }})", c);
      REQUIRE(rendered == "3");
   }

   {
      auto rendered = liquidpp::render(R"({{ 183.357 | round: 2 }})", c);
      REQUIRE(rendered == "183.36");
   }
}

TEST_CASE("Filter: rstrip")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "          So much room for activities!          " | rstrip }})", c);
      REQUIRE(rendered == "          So much room for activities!");
   }
}

TEST_CASE("Filter: slice")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Liquid" | slice: 0 }})", c);
      REQUIRE(rendered == "L");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Liquid" | slice: 2 }})", c);
      REQUIRE(rendered == "q");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Liquid" | slice: 2, 5 }})", c);
      REQUIRE(rendered == "quid");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Liquid" | slice: -3, 2 }})", c);
      REQUIRE(rendered == "ui");
   }
}

TEST_CASE("Filter: strip")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "          So much room for activities!          " | strip }})", c);
      REQUIRE(rendered == "So much room for activities!");
   }
}

TEST_CASE("Filter: strip_html")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Have <em>you</em> read <strong>Ulysses</strong>?" | strip_html }})", c);
      REQUIRE(rendered == "Have you read Ulysses?");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Have <em>you</em> read <!--Ulysses-->?" | strip_html }})", c);
      REQUIRE(rendered == "Have you read ?");
   }
}

TEST_CASE("Filter: strip_newlines")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({% capture string_with_newlines %}
Hello
there
{% endcapture %}

{{ string_with_newlines | strip_newlines }})", c);
      auto expected = R"(

Hellothere)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: truncate")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncate: 20 }})", c);
      REQUIRE(rendered == "Ground control to...");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncate: 25, ", and so on" }})", c);
      REQUIRE(rendered == "Ground control, and so on");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncate: 20, "" }})", c);
      REQUIRE(rendered == "Ground control to Ma");
   }
}

TEST_CASE("Filter: truncatewords")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncatewords: 3 }})", c);
      REQUIRE(rendered == "Ground control to...");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncatewords: 3, "--" }})", c);
      REQUIRE(rendered == "Ground control to--");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Ground control to Major Tom." | truncatewords: 3, "" }})", c);
      REQUIRE(rendered == "Ground control to");
   }
}

TEST_CASE("Filter: url_encode")
{
   liquidpp::Context c;

   {
      auto rendered = liquidpp::render(R"({{ "john@liquid.com" | url_encode }})", c);
      REQUIRE(rendered == "john%40liquid.com");
   }

   {
      auto rendered = liquidpp::render(R"({{ "Tetsuro Takara" | url_encode }})", c);
      REQUIRE(rendered == "Tetsuro+Takara");
   }
}

TEST_CASE("Filter: split")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign beatles = "John, Paul, George, Ringo" | split: ", " %}

{% for member in beatles %}
  {{ member }}
{% endfor %})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(


  John

  Paul

  George

  Ringo
)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: join")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign beatles = "John, Paul, George, Ringo" | split: ", " %}

{{ beatles | join: " and " }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

John and Paul and George and Ringo)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: first")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign my_array = "apples, oranges, peaches, plums" | split: ", " %}

{{ my_array.first }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

apples)";
      REQUIRE(rendered == expected);
   }

   {
      auto templ = R"({% assign my_array = "zebra, octopus, giraffe, tiger" | split: ", " %}

{{ my_array.first }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

zebra)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: last")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign my_array = "apples, oranges, peaches, plums" | split: ", " %}

{{ my_array.last }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

plums)";
      REQUIRE(rendered == expected);
   }

   {
      auto templ = R"({% assign my_array = "zebra, octopus, giraffe, tiger" | split: ", " %}

{{ my_array.last }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

tiger)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: sort")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign my_array = "zebra, octopus, giraffe, Sally Snake" | split: ", " %}

{{ my_array | sort | join: ", " }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

Sally Snake, giraffe, octopus, zebra)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: reverse")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign my_array = "apples, oranges, peaches, plums" | split: ", " %}

{{ my_array | reverse | join: ", " }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

plums, peaches, oranges, apples)";
      REQUIRE(rendered == expected);
   }

   {
      auto templ = R"({{ "Ground control to Major Tom." | split: "" | reverse | join: "" }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(.moT rojaM ot lortnoc dnuorG)";
      REQUIRE(rendered == expected);
   }
}

TEST_CASE("Filter: uniq")
{
   liquidpp::Context c;

   {
      auto templ = R"({% assign my_array = "ants, bugs, bees, bugs, ants" | split: ", " %}

{{ my_array | uniq | join: ", " }})";
      auto rendered = liquidpp::render(templ, c);
      auto expected = R"(

ants, bugs, bees)";
      REQUIRE(rendered == expected);
   }
}
