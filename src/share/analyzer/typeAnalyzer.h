/*
 * Created by yorkin on 11/24/21.
 *
 * type analyzer.
 *
 * This phase plays an important role in semantic check.
 *  - Determine expressions prototype and value type(lvalue or rvalue)
 *
 *  - Type check for function/sub/Constructor call,assignment,array index,for-statement condition,looping condition,
 *      select-case expression,return expression,binary/unary operation
 *
 *  - Access control check. E.g. Static Sub in class can't access Non-static field
 *
 *  - Insert nodes for implicit conversion and report warning.
 *
 *  - Reject invalid conversion
 *
 *  - Reject initialization of abstract class
 *
 *  - Determine il-data-type of symbols and expressions, Store into AST.
 *
 *
 */

#ifndef EVOBASIC_TYPEANALYZER_H
#define EVOBASIC_TYPEANALYZER_H

#include "defaultVisitor.h"
#include "type.h"
#include "context.h"
#include "ast.h"

#include <utils/utils.h>
#include <loader/il.h>

namespace evoBasic{

    struct TypeAnalyzerArgs{
        CompileTimeContext *context = nullptr;
        type::Domain *domain = nullptr;
        ExpressionType *dot_prefix = nullptr;
        type::Domain *current_class_or_module = nullptr;
        type::Symbol *dot_expression_context = nullptr;
        type::Function *function = nullptr;
        type::Function *checking_function = nullptr;
        int checking_arg_index = 0;
        bool need_lookup = false;
    };

    class TypeAnalyzer : public Visitor<TypeAnalyzerArgs>{
    public:
        std::any visitGlobal(parseTree::Global *global_node, TypeAnalyzerArgs args) override;
        std::any visitModule(parseTree::Module *mod_node, TypeAnalyzerArgs args) override;
        std::any visitClass(parseTree::Class *cls_node, TypeAnalyzerArgs args) override;
        std::any visitFunction(parseTree::Function *func_node, TypeAnalyzerArgs args) override;
        std::any visitConstructor(parseTree::Constructor *constructor_node, TypeAnalyzerArgs args) override;
        std::any visitType(parseTree::Type *ty_node, TypeAnalyzerArgs args) override;
        std::any visitEnum(parseTree::Enum *em_node, TypeAnalyzerArgs args) override;
        std::any visitDim(parseTree::Dim *dim_node, TypeAnalyzerArgs args) override;
        std::any visitInterface(parseTree::Interface *interface_node, TypeAnalyzerArgs args) override;
        std::any visitExternal(parseTree::External *ext_node, TypeAnalyzerArgs args) override;
        std::any visitVariable(parseTree::Variable *var_node, TypeAnalyzerArgs args) override;
        std::any visitParameter(parseTree::Parameter *param_node, TypeAnalyzerArgs args) override;

        std::any visitMember(parseTree::Member *member_node, TypeAnalyzerArgs args) override;
        std::any visitStatement(parseTree::stmt::Statement *stmt_node, TypeAnalyzerArgs args) override;

        std::any visitLet(parseTree::stmt::Let *let_node, TypeAnalyzerArgs args) override;
        std::any visitSelect(parseTree::stmt::Select *select_node, TypeAnalyzerArgs args) override;
        std::any visitLoop(parseTree::stmt::Loop *loop_node, TypeAnalyzerArgs args) override;
        std::any visitIf(parseTree::stmt::If *ifstmt_node, TypeAnalyzerArgs args) override;
        std::any visitFor(parseTree::stmt::For *forstmt_node, TypeAnalyzerArgs args) override;
        std::any visitReturn(parseTree::stmt::Return *ret_node, TypeAnalyzerArgs args) override;
        std::any visitContinue(parseTree::stmt::Continue *cont_node, TypeAnalyzerArgs args) override;

        std::any visitParentheses(parseTree::expr::Parentheses *parentheses_node, TypeAnalyzerArgs args) override;
        std::any visitExprStmt(parseTree::stmt::ExprStmt *expr_stmt_node, TypeAnalyzerArgs args) override;
        std::any visitExpression(parseTree::expr::Expression *expr_node, TypeAnalyzerArgs args) override;
        std::any visitBinary(parseTree::expr::Binary *logic_node, TypeAnalyzerArgs args) override;
        std::any visitUnary(parseTree::expr::Unary *unit_node, TypeAnalyzerArgs args) override;
        std::any visitID(parseTree::expr::ID *id_node, TypeAnalyzerArgs args) override;
        std::any visitDot(parseTree::expr::Dot *dot_node, TypeAnalyzerArgs args) override;
        std::any visitIndex(parseTree::expr::Index *index_node, TypeAnalyzerArgs args) override;
        std::any visitAssign(parseTree::expr::Assign *assign_node, TypeAnalyzerArgs args) override;
        std::any visitCast(parseTree::expr::Cast *cast_node, TypeAnalyzerArgs args) override;
        std::any visitColon(parseTree::expr::Colon *colon_node, TypeAnalyzerArgs args) override;
        std::any visitCallee(parseTree::expr::Callee *callee_node, TypeAnalyzerArgs args) override;
        std::any visitArg(parseTree::expr::Argument *arg_node, TypeAnalyzerArgs args) override;
        std::any visitNew(parseTree::expr::New *new_node, TypeAnalyzerArgs args) override;
        std::any visitAnnotation(parseTree::Annotation *anno_node, TypeAnalyzerArgs args) override;
        std::any visitAnnotationUnit(parseTree::AnnotationUnit *unit_node, TypeAnalyzerArgs args) override;

        type::Prototype *check_binary_op_valid(Location *code,ConversionRules &rules,
                                               type::Prototype *lhs,type::Prototype *rhs,
                                               parseTree::Expression *lhs_node,parseTree::Expression *rhs_node,
                                               ast::Expression **ast_lhs,ast::Expression **ast_rhs);
        void check_access(Location *code_location,type::Symbol *target,type::Domain *current,type::Domain *current_class_or_module);
        void check_callee(ast::Call *ast_node, Location *location, parseTree::expr::Argument *argument, type::Function *target, TypeAnalyzerArgs args);
        void check_static_access(Location *code_location,ExpressionType *lhs,bool is_rhs_static);
        std::any visitDigit(parseTree::expr::Digit *digit_node, TypeAnalyzerArgs args) override;
        std::any visitDecimal(parseTree::expr::Decimal *decimal, TypeAnalyzerArgs args) override;
        std::any visitBoolean(parseTree::expr::Boolean *bl_node, TypeAnalyzerArgs args) override;
        std::any visitChar(parseTree::expr::Char *ch_node, TypeAnalyzerArgs args) override;
        std::any visitString(parseTree::expr::String *str_node, TypeAnalyzerArgs args) override;

        ast::Statement *visitStatementList(parseTree::stmt::Statement *stmt_list, TypeAnalyzerArgs args);

        ast::Member *visitAllMember(type::Domain *domain, parseTree::Member *member, TypeAnalyzerArgs args);

    };
}

#endif //EVOBASIC_TYPEANALYZER_H
