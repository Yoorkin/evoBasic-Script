//
// Created by yorkin on 7/12/21.
//

#include "AST.h"

namespace evoBasic{
    Node::Node(Tag tag,initializer_list<pair<Attr,any>> attribute,initializer_list<Node*> child)
        : tag(tag), child(child), attr(attribute.begin(), attribute.end()){}

    Node::Node(Tag tag,initializer_list<pair<Attr,any>> attribute)
        : tag(tag), attr(attribute.begin(), attribute.end()){}

    Node::Node(Tag tag,initializer_list<Node*> child)
        :tag(tag),child(child){}

    void Node::print(string prefix,string mark,bool isLast,ostream& out) {
        if(this==nullptr){
            out<<prefix<<mark<<"─nullptr";
            return;
        }
        out<<prefix<<mark<<"─"<<TagToString[(int)tag];
        if(!attr.empty()){
            out<<" {";
            bool firstAttr = true;
            for(auto x:attr){
                if(firstAttr)firstAttr=false;else out<<',';
                out<<AttrToString[(int)x.first]<<':';
                auto& target = x.second.type();
                if(target==typeid(string))out<<'"'<<any_cast<string>(x.second)<<'"';
                else if(target==typeid(int))out<<any_cast<int>(x.second);
                else if(target==typeid(bool))out<<(any_cast<bool>(x.second)?"true":"false");
            }
            out<<"}";
        }
        out<<endl;
        if(isLast)prefix+=' ';else prefix+=mark;
        for(int i=0;i<child.size();i++){
            if(child[i]==nullptr)continue;
            if(i!=child.size()-1){
                child[i]->print(prefix + "  ","│",false,out);
            }else{
                child[i]->print(prefix + "  ","└",true,out);
            }
        }
    }

    vector<string> Node::TagToString{
            "Module","Class","Function","Sub","Enum","Type","TypeField","Let","Variable","Annotation",
            "Statements","EnumMember",
            "If","ElseIf","Else","Loop","Select","Case","DefaultCase",
            "For","Return","Continue","ExitFor","ExitLoop","ExitFunc","ExitSub",
            "Callee","ParameterArg","GenericArg","Assign'='",
            "Dot'.'","Comma','",
            "SelfNeg'-'","SelfPot'+'",
            "Add'+'","Minus'-'","Mul'*'","Div'/'","FDiv'\\'",
            "EQ'=='","NE'<>'","GT'>'","GE'>='","LT'<'","LE'<='",
            "And","Or","Xor","Not",
            "ParameterList","Parameter","Generic","Locating",
            "ID","Digit","Decimal","String","Char",
            "ClassMember","ModuleMember",
            "Public","Private","Friend","Static","Virtual","Override"
    };

    vector<string> Node::AttrToString{
        "Type","Name","Value","ValueKind","IsByval","IsOptional","Lexeme"
    };

    Node::Node(Tag tag):tag(tag){}
}