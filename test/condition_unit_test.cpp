#include "catch.hpp"

#include <liquidpp.hpp>
#include <liquidpp/Expression.hpp>

using liquidpp::string_view;

namespace ConditionUnitTest {

namespace {
liquidpp::Context c;
}

template<typename T, typename U>
bool evaluate(T left, string_view operator_, U right) {
   liquidpp::Expression expr;
   expr.tokens = {liquidpp::toValue(left), liquidpp::Expression::toToken(operator_), liquidpp::toValue(right)};
   if (expr(c))
      return true;
   return false;
}

template<typename T, typename U>
void assertEvaluteTrue(T left, string_view operator_, U right) {
   if (!evaluate(left, operator_, right)) {
      std::ostringstream oss;
      oss << "Evaluated false: " << left << " " << operator_ << " " << right;
      FAIL(oss.str());
   }
}

template<typename T, typename U>
void assertEvaluteFalse(T left, string_view operator_, U right) {
   if (evaluate(left, operator_, right)) {
      std::ostringstream oss;
      oss << "Evaluated true: " << left << " " << operator_ << " " << right;
      FAIL(oss.str());
   }
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
   assertEvaluteTrue("bob", "contains", "o");
   assertEvaluteTrue("bob", "contains", "b");
   assertEvaluteTrue("bob", "contains", "bo");
   assertEvaluteTrue("bob", "contains", "ob");
   assertEvaluteTrue("bob", "contains", "bob");
   
   assertEvaluteFalse("bob", "contains", "bob2");
   assertEvaluteFalse("bob", "contains", "a");
   assertEvaluteFalse("bob", "contains", "---");
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

   TEST_CASE("test_contains_works_on_arrays
    @context = Liquid::Context.new
    @context['array'] = [1, 2, 3, 4, 5]
    array_expr = VariableLookup.new("array")

    assertEvaluteFalse(array_expr, "contains", 0
    assertEvaluteTrue(array_expr, "contains", 1
    assertEvaluteTrue(array_expr, "contains", 2
    assertEvaluteTrue(array_expr, "contains", 3
    assertEvaluteTrue(array_expr, "contains", 4
    assertEvaluteTrue(array_expr, "contains", 5
    assertEvaluteFalse(array_expr, "contains", 6
    assertEvaluteFalse(array_expr, "contains", "1"
  end

   TEST_CASE("test_contains_returns_false_for_nil_operands
    @context = Liquid::Context.new
    assertEvaluteFalse(VariableLookup.new('not_assigned'), "contains", '0'
    assertEvaluteFalse(0, "contains", VariableLookup.new('not_assigned')
  end
  */

TEST_CASE("test_contains_return_false_on_wrong_data_type")
{
   assertEvaluteFalse(1, "contains", 0); // first succeeded of the wrong reasons
   assertEvaluteFalse(1, "contains", 1);
}

TEST_CASE("test_contains_with_string_left_operand_coerces_right_operand_to_string")
{
   assertEvaluteTrue(" 1 ", "contains", 1);
   assertEvaluteFalse(" 1 ", "contains", 2);
}

/*
   TEST_CASE("test_or_condition
    condition = Condition.new(1, "==", 2)

    assert_equal false, condition.evaluate

    condition.or Condition.new(2, "==", 1)

    assert_equal false, condition.evaluate

    condition.or Condition.new(1, "==", 1)

    assert_equal true, condition.evaluate
  end

   TEST_CASE("test_and_condition
    condition = Condition.new(1, "==", 1)

    assert_equal true, condition.evaluate

    condition.and Condition.new(2, "==", 2)

    assert_equal true, condition.evaluate

    condition.and Condition.new(2, "==", 1)

    assert_equal false, condition.evaluate
  end

   TEST_CASE("test_should_allow_custom_proc_operator
    Condition.operators['starts_with'] = proc { |cond, left, right| left =~ %r{^#{right}} }

    assertEvaluteTrue("bob", 'starts_with', "b"
    assertEvaluteFalse("bob", 'starts_with', "o"
  ensure
    Condition.operators.delete 'starts_with'
  end

  TEST_CASE("test_left_or_right_may_contain_operators
    @context = Liquid::Context.new
    @context['one'] = @context['another'] = "gnomeslab-and-or-liquid"

    assertEvaluteTrue(VariableLookup.new("one"), "==", VariableLookup.new("another")
  end
*/

}
