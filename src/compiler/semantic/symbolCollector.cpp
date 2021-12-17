//
// Created by yorkin on 11/24/21.
//

#include "symbolCollector.h"

using namespace std;

namespace evoBasic{

    std::any SymbolCollector::visitGlobal(evoBasic::ast::Global *global, evoBasic::SymbolCollectorArgs args) {
        NotNull(global);
        args.domain = args.context->getGlobal();
        args.domain->setLocation(global->location);

        FOR_EACH(iter,global->member){
            auto object = visitMember(iter,args);
            if(!object.has_value())continue;
            auto symbol = any_cast<type::Symbol*>(object);
            if(is_name_valid(symbol->getName(), symbol->getLocation(), args.domain)){
                args.domain->add(symbol);
            }
        }

        return {};
    }

    std::any SymbolCollector::visitModule(ast::Module *mod_node, SymbolCollectorArgs args) {
        NotNull(mod_node);
        if(mod_node->has_error)return {};

        auto mod = new type::Module();
        auto name = any_cast<string>(visitID(mod_node->name,args));
        mod->setName(name);
        mod->setAccessFlag(mod_node->access);
        mod->setLocation(mod_node->location);
        args.domain = mod;
        mod_node->module_symbol = mod;

        FOR_EACH(iter,mod_node->member){
            auto object = visitMember(iter,args);
            if(!object.has_value())continue;
            auto symbol = any_cast<type::Symbol*>(object);
            if(is_name_valid(symbol->getName(), symbol->getLocation(), mod)){
                mod->add(symbol);
            }
        }

        return mod->as<type::Symbol*>();
    }

    std::any SymbolCollector::visitClass(ast::Class *cls_node, SymbolCollectorArgs args) {
        NotNull(cls_node);
        if(cls_node->has_error)return {};

        auto cls = new type::Class;
        cls->setName(any_cast<string>(visitID(cls_node->name,args)));
        cls->setAccessFlag(cls_node->access);
        cls->setLocation(cls_node->location);
        args.domain = cls;
        cls_node->class_symbol = cls;

        FOR_EACH(iter,cls_node->member){
            auto object = visitMember(iter,args);
            if(!object.has_value())continue;
            auto symbol = any_cast<type::Symbol*>(object);
            if(is_name_valid(symbol->getName(), symbol->getLocation(), cls)){
                cls->add(symbol);
            }
        }
        return cls->as<type::Symbol*>();
    }


    std::any SymbolCollector::visitInterface(ast::Interface *interface_node, SymbolCollectorArgs args) {
        auto interface = new type::Interface;
        interface->setName(getID(interface_node->name));
        interface->setAccessFlag(interface_node->access);
        interface->setStatic(interface_node->is_static);
        interface->setLocation(interface_node->location);
        interface_node->interface_symbol = interface;
        return interface->as<type::Symbol*>();
    }

    std::any SymbolCollector::visitEnum(ast::Enum *em_node, SymbolCollectorArgs args) {
        NotNull(em_node);
        if(em_node->has_error)return {};

        auto em = new type::Enumeration;
        em->setName(any_cast<string>(visitID(em_node->name,args)));
        em->setAccessFlag(em_node->access);
        em->setLocation(em_node->location);
        em_node->enum_symbol = em;
        return em->as<type::Symbol*>();
    }

    std::any SymbolCollector::visitType(ast::Type *type_node, SymbolCollectorArgs args) {
        NotNull(type_node);
        auto ty = new type::Record;
        ty->setName(any_cast<string>(visitID(type_node->name,args)));
        ty->setAccessFlag(type_node->access);
        ty->setLocation(type_node->location);
        type_node->type_symbol = ty;

        FOR_EACH(iter,type_node->member){
            auto symbol = any_cast<type::Symbol*>(visitVariable(iter,args));
            symbol->setAccessFlag(AccessFlag::Public);
            if(is_name_valid(symbol->getName(), iter->location,ty)){
                ty->add(symbol);
            }
        }
        return ty->as<type::Symbol*>();
    }

    std::any SymbolCollector::visitDim(ast::Dim *dim_node, SymbolCollectorArgs args) {
        NotNull(dim_node);
        type::Symbol *symbol = nullptr;

        FOR_EACH(iter,dim_node->variable){
            symbol = any_cast<type::Symbol*>(visitVariable(iter,args));
            symbol->setAccessFlag(dim_node->access);
            symbol->setStatic(dim_node->is_static);
            symbol->as<type::Variable*>()->setConstant(dim_node->is_const);
            if(is_name_valid(symbol->getName(), symbol->getLocation(), args.domain)){
                args.domain->add(symbol);
            }
        }
        return {};
    }

    std::any SymbolCollector::visitVariable(ast::Variable *variable_node, SymbolCollectorArgs args) {
        NotNull(variable_node);
        auto variable = new type::Variable;
        auto name = any_cast<string>(visitID(variable_node->name, args));
        variable->setName(name);
        variable->setLocation(variable_node->location);
        variable_node->variable_symbol = variable;
        return variable->as<type::Symbol*>();
    }

    std::any SymbolCollector::visitID(ast::expr::ID *id, SymbolCollectorArgs args) {
        NotNull(id);
        return id->lexeme;
    }

    std::any SymbolCollector::visitMember(ast::Member *member_node, SymbolCollectorArgs args) {
        switch (member_node->member_kind) {
            case ast::Member::class_:    return visitClass((ast::Class*)member_node,args);
            case ast::Member::module_:   return visitModule((ast::Module*)member_node,args);
            case ast::Member::type_:     return visitType((ast::Type*)member_node,args);
            case ast::Member::enum_:     return visitEnum((ast::Enum*)member_node,args);
            case ast::Member::dim_:      return visitDim((ast::Dim*)member_node,args);
            case ast::Member::interface_:return visitInterface((ast::Interface*)member_node,args);
        }
        return {};
    }


}