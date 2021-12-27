//
// Created by yorkin on 12/11/21.
//

#ifndef EVOBASIC_IL_H
#define EVOBASIC_IL_H
#include <string>
#include <ostream>
#include <vector>
#include <any>
#include <map>
#include <data.h>
#include <parseTree.h>
#include <list>
#include <type.h>

namespace evoBasic::il{
    class Class;
    class Interface;
    class Inst;
    class SFtn;
    class VFtn;
    class Block;
    class FtnBase;

    class Node {
    public:
        virtual std::string toString()=0;
        virtual void toHex(std::ostream &stream)=0;
    };

    class Token : public Node{
        std::string text;
    protected:
        data::u64 id = -1;
    public:
        Token()=default;
        explicit Token(std::string text) : text(text){}
        void toHex(std::ostream &stream)override;
        std::string toString()override;
        virtual std::string getName();
        void setID(data::u64 value);
        data::u64 getID();
    };

    class ConstructedToken : public Token{
    public:
        std::vector<Token*> tokens;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
        std::string getName()override;
    };

    class Access : public Node{
        AccessFlag flag = AccessFlag::Private;
    public:
        explicit Access(AccessFlag flag) : flag(flag){}
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Extend : public Node{
    public:
        Token *target = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Impl : public Node{
    public:
        Token *target = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Lib : public Node{
    public:
        explicit Lib(Token *target) : target(target){}
        Token *target = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Member : public Node{
        ConstructedToken *path_token = nullptr;
    public:
        Token *name = nullptr;
        Access *access = nullptr;
        Member *parent = nullptr;
        ConstructedToken *getConstructedToken();
    };

    class Class : public Member{
    public:
        Extend *extend = nullptr;
        std::vector<Impl*> impls;
        std::vector<Member*> members;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Module : public Member{
    public:
        std::vector<Member*> members;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Interface : public Member{
    public:
        std::vector<FtnBase*> ftns;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Pair : public Node{
    public:
        Token *name = nullptr;
        data::u32 value;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Enum : public Member{
    public:
        std::vector<Pair*> pairs;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Fld : public Member{
    public:
        Token *type = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class SFld : public Fld{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Record : public Member{
    public:
        std::vector<Fld*> fields;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Param : public Node{
    public:
        bool is_ref = false;
        Token *name = nullptr,*type = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Opt : public Param{
    public:
        Block *initial = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Inf : public Param{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Local : public Node{
    public:
        data::u16 address = -1;
        Token *name = nullptr,*type = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Result : public Node{
    public:
        Token *type = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class FtnBase : public Member{
    public:
        std::vector<Param*> params;
        Result *result = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class FtnWithDefinition : public FtnBase{
    public:
        std::vector<Local*> locals;
        std::string toString()override;
        std::list<Block*> blocks;
        void toHex(std::ostream &stream)override;
    };

    class Ctor : public FtnWithDefinition{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Ftn : public FtnWithDefinition{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class VFtn : public FtnWithDefinition{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class SFtn : public FtnWithDefinition{
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class ExtAlias : public Node{
        Token *token = nullptr;
    public:
        explicit ExtAlias(Token *token) : token(token){}
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Ext : public FtnBase{
    public:
        Lib *lib = nullptr;
        ExtAlias *alias = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };


    class Inst : public Node{};

    class InstWithOp : public Inst{
    public:
        enum Op{Nop,Ret,CallVirt,CallExt,Callstatic,Call,Ldnull,And,Or,Xor,Ldloca,Ldarga,Ldelema,Not}op;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
        InstWithOp(Op op):op(op){}
    };

    class InstWithToken : public Inst{
    public:
        enum Op{Ldftn,Ldsftn,Ldvftn,Ldc,Newobj,Invoke,Ldflda,Ldsflda}op;
        InstWithToken(Op op,Token *token):op(op),token(token){}
        Token *token = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class InstJif : public Inst{
    public:
        Block *target = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    struct InstBr : public Inst{
    public:
        Block *target = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class InstPush : public Inst{
    public:
        DataType type;
        std::any value;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class InstWithData : public Inst{
    public:
        enum Op{Ldelem,Stelem,Stelema,Ldarg,Starg,Ldloc,Stloc,
                Add,Sub,Mul,Div,FDiv,EQ,NE,LT,GT,LE,GE,Neg,Pop,Dup}op;
        DataType type;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
        InstWithData(Op op,DataType type):op(op),type(type){}
    };

    class InstWithDataToken : public Inst{
    public:
        enum Op{Ldfld,Ldsfld,Stfld,Stsfld}op;
        InstWithDataToken(Op op,DataType type,Token *token) : op(op),type(type),token(token){}
        DataType type;
        Token *token = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class InstCastcls : public Inst{
    public:
        Token *src_class = nullptr,
              *dst_class = nullptr;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };


    class InstConv : public Inst{
    public:
        DataType src,dst;
        std::string toString()override;
        void toHex(std::ostream &stream)override;
    };

    class Document : public Node{
        std::map<std::string,int> token_pool_map;
        std::vector<Token*> token_pool;
        std::vector<ConstructedToken*> constructed_token_pool;
        std::vector<Member*> members;
    public:
        std::string toString()override;
        void toHex(std::ostream &stream)override;

        Token *createToken(std::string text);
        Token *createConstructedToken(std::list<std::string> full_name_list);

        Module *createModule(std::string name,AccessFlag access,std::vector<Member*> members);
        Class *createClass(std::string name,AccessFlag access,Extend *extend,std::vector<Impl*> impls,std::vector<Member*> members);
        Interface *createInterface(std::string name,AccessFlag access,std::vector<FtnBase*> ftns);
        Enum *createEnum(std::string name,AccessFlag access,std::vector<Pair*> pairs);
        Record *createRecord(std::string name,AccessFlag access,std::vector<Fld*> fields);
        Fld *createField(std::string name,AccessFlag access,type::Prototype *prototype);
        SFld *createStaticField(std::string name,AccessFlag access,type::Prototype *prototype);

        Ftn *createFunction(std::string name,AccessFlag access,std::vector<Param*> params,Result *result,std::vector<Local*> locals,std::list<Block*> blocks);
        Ctor *createConstructor(AccessFlag access,std::vector<Param*> params,std::vector<Local*> locals,std::list<Block*> blocks);
        VFtn *createVirtualFunction(std::string name,AccessFlag access,std::vector<Param*> params,Result *result,std::vector<Local*> locals,std::list<Block*> blocks);
        SFtn *createStaticFunction(std::string name,AccessFlag access,std::vector<Param*> params,Result *result,std::vector<Local*> locals,std::list<Block*> blocks);

        FtnBase *createInterfaceFunction(std::string name,AccessFlag access,std::vector<Param*> params,Result *result);
        Ext *createExternalFunction(std::string name,std::string lib,ExtAlias *alias,AccessFlag access,std::vector<Param*> params,Result *result);

        Param *createParam(std::string name,type::Prototype *prototype,bool byref);
        Opt *createOption(std::string name,type::Prototype *prototype,bool byref,Block *initial);
        Inf *createParamArray(std::string name,type::Prototype *prototype,bool byref);
        Pair *createPair(std::string name,data::u32 value);
        Extend *createExtend(type::Class *cls);
        Impl *createImplements(type::Interface *interface);
        Local *createLocal(std::string name,type::Prototype *prototype, data::u16 address);
        Result *createResult(type::Prototype *prototype);

        void add(Member* member);

        ExtAlias *createExtAlias(std::string text);
    };


    class Block : public Node{
    public:
        data::u32 getAddress();
        std::string toString()override;
        void toHex(std::ostream &stream)override;
        std::vector<Inst*> insts;
        Block(std::initializer_list<Inst*> inst_init);
        Block()=default;
        Block &Br(Block *block);
        Block &Jif(Block *block);
        Block &EQ(DataType data);
        Block &NE(DataType data);
        Block &LT(DataType data);
        Block &GT(DataType data);
        Block &LE(DataType data);
        Block &GE(DataType data);
        Block &Add(DataType data);
        Block &Sub(DataType data);
        Block &Mul(DataType data);
        Block &Div(DataType data);
        Block &FDiv(DataType data);
        Block &Neg(DataType data);
        Block &And();
        Block &Or();
        Block &Xor();
        Block &Not();
        Block &Nop();
        Block &Pop(DataType data);
        Block &Dup(DataType data);
        Block &Ret();
        Block &Push(DataType data, std::any value);
        Block &Ldc(Token *token);
        Block &Ldftn(Token *ftn);
        Block &Ldsftn(Token *sftn);
        Block &Ldvftn(Token *vftn);
        Block &Ldarg(DataType data);
        Block &Starg(DataType data);
        Block &Ldarga();
        Block &Ldloc(DataType data);
        Block &Ldloca();
        Block &Stloc(DataType data);
        Block &Ldfld(DataType data,Token *fld);
        Block &Ldsfld(DataType data,Token *sfld);
        Block &Ldflda(Token *fld);
        Block &Ldsflda(Token *sfld);
        Block &Stfld(DataType data,Token *fld);
        Block &Stsfld(DataType data,Token *sfld);
        Block &Ldelem(DataType data);
        Block &Ldelema();
        Block &Stelem(DataType data);
        Block &Ldnull();
        Block &Newobj(Token *cls);
        Block &Castcls(Token *src, Token *dst);
        Block &Conv(DataType src, DataType dst);
        Block &Callvirt();
        Block &Invoke(Token *external);
        Block &Callstatic();
        Block &Call();
    };


    template<typename T>
    void write(std::ostream &stream,const T t){
        stream.write((const char*)(&t),sizeof(t));
    }

}

#endif //EVOBASIC_IL_H
