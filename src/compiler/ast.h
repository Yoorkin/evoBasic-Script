#ifndef EB_AST
#define EB_AST
#include "token.h"
#include "data.h"
#include "nullSafe.h"
#include <memory>

namespace evoBasic{
    enum class AccessFlag {Public,Private,Friend,Protected};
    enum class MethodFlag {Static,Virtual,Override,None};
    struct ExpressionType;
    namespace type{
        class Variable;
    }
}

namespace evoBasic::ast{

    struct Node;
    struct Global;
    struct Class;
    struct Module;
    struct Member;
    struct Import;
    struct Dim;
    struct Variable;
    struct Function;
    struct External;
    struct Init;
    struct Operator;
    struct Enum;
    struct Type;
    struct Parameter;
    struct Annotation;
    struct Generic;
    struct Implement;

    namespace stmt{
        struct Statement;
        struct Let;
        struct Select;
        struct Loop;
        struct If;
        struct For;
        struct Control;
        struct Return;
        struct Exit;
    }

    struct Case;

    namespace expr{
        struct Expression;
        struct Binary;
        struct Unary;
        struct Terminal;
        struct Callee;
        struct ID;
        struct Digit;
        struct Decimal;
        struct String;
        struct Char;
    }



    std::string debugAST(Node *ast);

    using Expr = expr::Expression;

    struct DebugInfo{
        std::string text;
        DebugInfo *child = nullptr,*sibling = nullptr;

        ~DebugInfo(){
            while(sibling!=nullptr){
                auto head = sibling;
                sibling = head->sibling;
                delete head;
            }
            delete child;
        }
    };

    struct Node{
        Location *location = nullptr;
        virtual DebugInfo *debug()=0;
    };

    struct Global : Node{
        Member *member = nullptr;
        DebugInfo *debug()override;
    };

    struct Member : Node{
        AccessFlag access;
        enum MemberKind{
            error,function_,class_,module_,type_,enum_,dim_,operator_,init_,
            import_,external_
        }member_kind = error;

        Member *next_sibling = nullptr,*prv_sibling = nullptr;
    };

    struct Class : Member{
        Class(){member_kind = MemberKind::class_;}
        expr::ID *name = nullptr;
        Annotation *extend = nullptr;
        Implement *impl = nullptr;

        Member *member = nullptr;
        DebugInfo *debug()override;
    };

    struct Module : Member{
        Module(){member_kind = MemberKind::module_;}
        expr::ID *name = nullptr;
        Member *member = nullptr;
        DebugInfo *debug()override;
    };

    struct Implement : Node {
        Annotation* annotation = nullptr;
        Implement *next_sibling = nullptr,
                  *prv_sibling = nullptr;
        DebugInfo *debug()override;
    };

    struct Import : Member{
        Import(){member_kind = MemberKind::import_;}
        Annotation *annotation = nullptr;
        DebugInfo *debug()override;
    };

    struct Dim : Member{
        Dim(){member_kind = MemberKind::dim_;}
        bool is_const = false;
        Variable* variable;
        DebugInfo *debug()override;
    };

    struct Variable : Node{
        expr::ID *name = nullptr;
        Annotation *annotation = nullptr;
        expr::Expression *initial = nullptr;
        DebugInfo *debug()override;
        Variable *next_sibling = nullptr,*prv_sibling = nullptr;
    };

    struct Function : Member{
        Function(){member_kind = MemberKind::function_;}
        MethodFlag method_flag;
        expr::ID *name = nullptr;
        Parameter *parameter = nullptr;
        Annotation *return_annotation = nullptr;
        stmt::Statement* statement = nullptr;
        DebugInfo *debug()override;
    };

    struct External : Member{
        External(){member_kind = MemberKind::external_;}
        expr::ID *name = nullptr;
        expr::String *lib = nullptr,*alias = nullptr;
        Parameter* parameter = nullptr;
        Annotation *return_annotation = nullptr;
        DebugInfo *debug()override;
    };

    struct EnumMember : Node {
        expr::ID *name = nullptr;
        expr::Digit *value = nullptr;
        EnumMember *next_sibling = nullptr,*prv_sibling = nullptr;
        DebugInfo *debug()override;
    };

    struct Enum : Member{
        Enum(){member_kind = MemberKind::enum_;}
        expr::ID *name = nullptr;
        EnumMember *member = nullptr;
        DebugInfo *debug()override;
    };


    struct Type : Member{
        Type(){member_kind = MemberKind::type_;}
        expr::ID *name = nullptr;
        Variable *member = nullptr;
        DebugInfo *debug()override;
    };


    struct Parameter : Node{
        bool is_byval = false;
        bool is_optional = false;
        expr::ID *name = nullptr;
        Annotation *annotation = nullptr;
        DebugInfo *debug()override;
        Parameter *next_sibling = nullptr,*prv_sibling = nullptr;
    };

    namespace stmt{

        struct Statement : Node{
            enum Enum{
                error,let_,loop_,if_,for_,select_,return_,continue_,exit_,expr_
            } stmt_flag=error;
            Statement *next_sibling = nullptr,*prv_sibling = nullptr;
        };

        struct Let : Statement{
            Let(){stmt_flag = let_;}
            bool is_const = false;
            Variable* variable = nullptr;
            DebugInfo *debug()override;
        };

        struct Select : Statement{
            Select(){stmt_flag = select_;}
            Expr *condition = nullptr;
            Case *case_ = nullptr;
            DebugInfo *debug()override;
        };

        struct Loop : Statement{
            Loop(){stmt_flag = loop_;}
            Expr *condition = nullptr;
            Statement* statement = nullptr;
            DebugInfo *debug()override;
        };

        struct If : Statement{
            If(){stmt_flag = if_;}
            Case *case_ = nullptr;
            DebugInfo *debug()override;
        };

        struct For : Statement{
            For(){stmt_flag = for_;}
            expr::Expression *iterator = nullptr;
            bool iterator_has_let = false;
            Expr *begin = nullptr,*end = nullptr,*step = nullptr;
            Statement* statement = nullptr;
            DebugInfo *debug()override;
        };

        struct Return : Statement{
            Return(){stmt_flag = return_;}
            Expr *expr = nullptr;
            DebugInfo *debug()override;
        };

        struct Exit : Statement{
            Exit(){stmt_flag = exit_;}
            enum {For,While,Sub}exit_flag;
            DebugInfo *debug()override;
        };

        struct Continue : Statement{
            Continue(){stmt_flag = continue_;}
            DebugInfo *debug()override;
        };

        struct ExprStmt : Statement{
            ExprStmt(){stmt_flag = expr_;}
            Expr *expr;
            DebugInfo *debug()override;
        };
    }

    struct Case : Node{
        Expr *condition = nullptr;
        stmt::Statement *statement = nullptr;
        Case *next_sibling = nullptr,*prv_sibling = nullptr;
        DebugInfo *debug()override;
    };

    struct AnnotationUnit : Node{
        expr::ID *name = nullptr;
        DebugInfo *debug()override;
        AnnotationUnit *next_sibling = nullptr,*prv_sibling = nullptr;
    };

    struct Annotation : Node{
        AnnotationUnit* unit = nullptr;
        expr::Digit *array_size = nullptr;
        DebugInfo *debug()override;
    };

    namespace expr{

        struct Expression : Node{
            enum ExpressionKind{
                error_ = 0,binary_,unary_,parentheses_,ID_,cast_,
                digit_,decimal_,string_,char_,boolean_,callee_
            }expression_kind = error_;
            DebugInfo *debug()override;
            ExpressionType *type = nullptr;
        };

        struct Binary : Expression{
            Binary(){expression_kind = ExpressionKind::binary_;}
            enum Enum{
                Empty,And,Or,Xor,Not,EQ,NE,GE,LE,GT,LT,
                ADD,MINUS,MUL,DIV,FDIV,ASSIGN,Dot,Index
            };
            Binary(Expression *lhs,Enum op,Expression *rhs):Binary(){
                NotNull(lhs);
                NotNull(rhs);
                this->lhs = lhs;
                this->rhs = rhs;
                this->op = op;
                this->location = new Location(lhs->location,rhs->location);
            }
            Expression *lhs = nullptr;
            Enum op = Empty;
            Expression *rhs = nullptr;
            std::shared_ptr<type::Variable> temp_address = nullptr;
            DebugInfo *debug()override;
        };

        struct Cast : Expression{
            Cast(Expression *expression,Annotation *annotation){
                expression_kind = cast_;
                this->expr = expression;
                this->annotation = annotation;
            }
            Expression *expr = nullptr;
            Annotation *annotation = nullptr;
            DebugInfo *debug()override;
        };

        struct Unary : Expression{
            Unary(){expression_kind = ExpressionKind::unary_;}
            enum Enum{Empty,MINUS,ADD}op = Empty;
            Expression *terminal = nullptr;
            Unary(Enum op,Expression *terminal):Unary(){
                this->op = op;
                this->terminal = terminal;
                this->location = terminal->location;
            }
            DebugInfo *debug()override;
        };

        struct Parentheses : Expression{
            Parentheses(){
                expression_kind = ExpressionKind::parentheses_;
            }
            Expression *expr = nullptr;
            DebugInfo *debug()override;
        };

        struct ID : Expression{
            ID(){
                expression_kind = ExpressionKind::ID_;
            }
            std::string lexeme;
            DebugInfo *debug()override;
        };

        struct Callee : Expression{
            struct Argument : Node{
                enum PassKind{undefined,byref,byval}pass_kind = undefined;
                std::shared_ptr<type::Variable> temp_address = nullptr;
                expr::Expression *expr = nullptr;
                Argument *next_sibling = nullptr,*prv_sibling = nullptr;
                DebugInfo *debug()override;
            };

            Callee(){
                expression_kind = ExpressionKind::callee_;
            }
            ID *name = nullptr;
            Argument* argument = nullptr;
            DebugInfo *debug()override;
        };

        struct Digit : Expression{
            Digit(){
                expression_kind = ExpressionKind::digit_;
            }
            data::i32 value = 0;
            DebugInfo *debug()override;
        };

        struct Decimal : Expression{
            Decimal(){
                expression_kind = ExpressionKind::decimal_;
            }
            data::f64 value = 0;
            DebugInfo *debug()override;
        };

        struct String : Expression{
            String(){
                expression_kind = ExpressionKind::string_;
            }
            std::string value;
            DebugInfo *debug()override;
        };

        struct Char : Expression{
            Char(){
                expression_kind = ExpressionKind::char_;
            }
            data::i8 value = 0;
            DebugInfo *debug()override;
        };

        struct Boolean : Expression{
            Boolean(){
                expression_kind = ExpressionKind::boolean_;
            }
            data::boolean value = false;
            DebugInfo *debug()override;
        };
    }

}



#endif