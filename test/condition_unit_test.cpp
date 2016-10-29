#include "catch.hpp"

#include <liquidpp.hpp>
#include <liquidpp/Expression.hpp>

using liquidpp::string_view;

namespace ConditionUnitTest {

liquidpp::Expression::Token toToken(int val)
{
   return liquidpp::toValue(val);
}

liquidpp::Expression::Token toToken(double val)
{
   return liquidpp::toValue(val);
}

liquidpp::Expression::Token toToken(liquidpp::Value val)
{
   return val;
}

liquidpp::Expression::Token toToken(liquidpp::string_view val)
{
   return liquidpp::Expression::toToken(val);
}

template<typename... Args>
bool evaluate(liquidpp::Context& c, Args... args) {
   liquidpp::Expression expr;
   expr.tokens = {toToken(args)...};
   if (expr(c))
      return true;
   return false;
}

template<typename... Args>
bool evaluate(Args... args) {
   liquidpp::Context c;
   return evaluate(c, std::forward<Args>(args)...);
}

template<typename T, typename U>
void assertEvaluteTrue(liquidpp::Context& c, T left, string_view operator_, U right) {
   if (!evaluate(c, left, operator_, right)) {
      std::ostringstream oss;
      oss << "Evaluated false: " << left << " " << operator_ << " " << right;
      FAIL(oss.str());
   }
}

template<typename T, typename U>
void assertEvaluteTrue(T left, string_view operator_, U right) {
   liquidpp::Context c;
   return assertEvaluteTrue(c, left, operator_, right);
}

template<typename T, typename U>
void assertEvaluteFalse(liquidpp::Context& c, T left, string_view operator_, U right) {
   if (evaluate(c, left, operator_, right)) {
      std::ostringstream oss;
      oss << "Evaluated true: " << left << " " << operator_ << " " << right;
      FAIL(oss.str());
   }
}

template<typename T, typename U>
void assertEvaluteFalse(T left, string_view operator_, U right) {
   liquidpp::Context c;
   return assertEvaluteFalse(c, left, operator_, right);
}

template<typename T, typename U>
void assertEvaluteArgumentError(T left, string_view op, U right)
{
   REQUIRE_THROWS(evaluate(left, op, right));
}

TEST_CASE("test_basic_condition") {
   REQUIRE(false == evaluate(1, "==", 2));
   REQUIRE(true == evaluate(1, "==", 1));
}

TEST_CASE("test_default_operators_evalute_true") {
   assertEvaluteTrue(1, "==", 1);
   assertEvaluteTrue(1, "!=", 2);
   assertEvaluteTrue(1, "<>", 2);
   assertEvaluteTrue(1, "<", 2);
   assertEvaluteTrue(2, ">", 1);
   assertEvaluteTrue(1, ">=", 1);
   assertEvaluteTrue(2, ">=", 1);
   assertEvaluteTrue(1, "<=", 2);
   assertEvaluteTrue(1, "<=", 1);
   // negative numbers
   assertEvaluteTrue(1, ">", -1);
   assertEvaluteTrue((-1), "<", 1);
   assertEvaluteTrue(1.0, ">", -1.0);
   assertEvaluteTrue((-1.0), "<", 1.0);
}

TEST_CASE("test_default_operators_evalute_false") {
   assertEvaluteFalse(1, "==", 2);
   assertEvaluteFalse(1, "!=", 1);
   assertEvaluteFalse(1, "<>", 1);
   assertEvaluteFalse(1, "<", 0);
   assertEvaluteFalse(2, ">", 4);
   assertEvaluteFalse(1, ">=", 3);
   assertEvaluteFalse(2, ">=", 4);
   assertEvaluteFalse(1, "<=", 0);
   assertEvaluteFalse(1, "<=", 0);
}

TEST_CASE("test_contains_works_on_strings")
{
   assertEvaluteTrue("'bob'", "contains", "'o'");
   assertEvaluteTrue("'bob'", "contains", "'b'");
   assertEvaluteTrue("'bob'", "contains", "'bo'");
   assertEvaluteTrue("'bob'", "contains", "'ob'");
   assertEvaluteTrue("'bob'", "contains", "'bob'");
   
   assertEvaluteFalse("'bob'", "contains", "'bob2'");
   assertEvaluteFalse("'bob'", "contains", "'a'");
   assertEvaluteFalse("'bob'", "contains", "'---'");
}

TEST_CASE("test_invalid_comparation_operator")
{
   assertEvaluteArgumentError(1, "~~", 0);
}

/* TODO
   // why exactley is this required to fail? (numeric operator on string OR string <-> int)
     TEST_CASE("test_comparation_of_int_and_str
    assertEvaluteArgumentError "1", ">", 0
    assertEvaluteArgumentError "1", "<", 0
    assertEvaluteArgumentError "1", ">=", 0
    assertEvaluteArgumentError "1", "<=", 0
  end
*/

TEST_CASE("test_contains_works_on_arrays")
{
   liquidpp::Context context;
   context.set("array", std::vector<int>{1, 2, 3, 4, 5});

   assertEvaluteFalse(context, "array", "contains", 0);
   assertEvaluteTrue(context, "array", "contains", 1);
   assertEvaluteTrue(context, "array", "contains", 2);
   assertEvaluteTrue(context, "array", "contains", 3);
   assertEvaluteTrue(context, "array", "contains", 4);
   assertEvaluteTrue(context, "array", "contains", 5);
   assertEvaluteFalse(context, "array", "contains", 6);
   
   SECTION("number array and string needle")
   {
      assertEvaluteFalse(context, "array", "contains", "'1'");
   }
}

TEST_CASE("test_contains_returns_false_for_nil_operands")
{
   assertEvaluteFalse("not_assigned", "contains", "0");
   assertEvaluteFalse(0, "contains", "not_assigned");
}

TEST_CASE("test_contains_return_false_on_wrong_data_type")
{
   assertEvaluteFalse(1, "contains", 0); // first succeeded of the wrong reasons
   assertEvaluteFalse(1, "contains", 1);
}

TEST_CASE("test_contains_with_string_left_operand_coerces_right_operand_to_string")
{
   assertEvaluteTrue("' 1 '", "contains", 1);
   assertEvaluteFalse("' 1 '", "contains", 2);
}

TEST_CASE("test_or_condition")
{
   REQUIRE_FALSE(evaluate(1, "==", 2));

   REQUIRE_FALSE(evaluate(1, "==", 2, "or", 2, "==", 1));

   REQUIRE(evaluate(1, "==", 2, "or", 2, "==", 1, "or", 1, "==", 1));
}

TEST_CASE("test_and_condition")
{
    REQUIRE(evaluate(1, "==", 1));

    REQUIRE(evaluate(1, "==", 1, "and", 2, "==", 2));

    REQUIRE_FALSE(evaluate(1, "==", 1, "and", 2, "==", 2, "and", 2, "==", 1));
}

// User defined opeators are not supported and I don't plan to change this. The 'starts_with' operator may be supported sometimes
/*
   TEST_CASE("test_should_allow_custom_proc_operator
    Condition.operators['starts_with'] = proc { |cond, left, right| left =~ %r{^#{right}} }

    assertEvaluteTrue("bob", 'starts_with', "b"
    assertEvaluteFalse("bob", 'starts_with', "o"
  ensure
    Condition.operators.delete 'starts_with'
  end
*/

TEST_CASE("test_left_or_right_may_contain_operators")
{
    liquidpp::Context context;
    context.set("one", "gnomeslab-and-or-liquid");
    context.set("another", "gnomeslab-and-or-liquid");
    context.set("another1", "gnomeslab-and-or-liquid1");

    assertEvaluteTrue(context, "one", "==", "another");
    assertEvaluteFalse(context, "one", "==", "another1");
}

}
