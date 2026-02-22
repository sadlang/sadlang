/**
 * @file scope_manager.cpp
 * @brief (AR) تنفيذ مدير النطاقات
 * @brief (EN) Scope manager implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#include "../../include/managers/scope_manager.h"
#include <sstream>
#include <iostream>

namespace Sad {
namespace Data {

// ========================================
// Scope Class Implementation
// ========================================

Scope::Scope(ScopeType type, const std::string& name, Scope* parent)
    : type_(type), name_(name), parent_(parent) {
}

bool Scope::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void Scope::declareVariable(const std::string& name) {
    variables_[name] = true;
}

bool Scope::isVariableDeclared(const std::string& name) const {
    return hasVariable(name);
}

int Scope::getDepth() const {
    int depth = 0;
    Scope* current = parent_;
    while (current != nullptr) {
        depth++;
        current = current->parent_;
    }
    return depth;
}

std::string Scope::getTypeName() const {
    switch (type_) {
        case ScopeType::GLOBAL:   return "GLOBAL";
        case ScopeType::FUNCTION: return "FUNCTION";
        case ScopeType::BLOCK:    return "BLOCK";
        case ScopeType::LOOP:     return "LOOP";
        case ScopeType::LAMBDA:   return "LAMBDA";
        case ScopeType::CLASS:    return "CLASS";
        default:                  return "UNKNOWN";
    }
}

std::string Scope::debugString() const {
    std::ostringstream oss;
    oss << "Scope[" << getTypeName();
    if (!name_.empty()) {
        oss << " '" << name_ << "'";
    }
    oss << ", depth=" << getDepth();
    oss << ", vars=" << variables_.size();
    oss << "]";
    return oss.str();
}

// ========================================
// ScopeManager Class Implementation
// ========================================

ScopeManager::ScopeManager() {
    // Create global scope
    auto globalScopePtr = std::make_unique<Scope>(ScopeType::GLOBAL, "global", nullptr);
    globalScope_ = globalScopePtr.get();
    currentScope_ = globalScope_;
    scopeStack_.push_back(std::move(globalScopePtr));
}

ScopeManager::~ScopeManager() {
    // unique_ptr will automatically clean up
}

void ScopeManager::pushScope(ScopeType type, const std::string& name) {
    // Create new scope with current scope as parent
    auto newScopePtr = std::make_unique<Scope>(type, name, currentScope_);
    Scope* newScope = newScopePtr.get();
    
    // Add to stack and update current
    scopeStack_.push_back(std::move(newScopePtr));
    currentScope_ = newScope;
}

void ScopeManager::popScope() {
    // Cannot pop global scope
    if (currentScope_ == globalScope_) {
        throwError(
            "لا يمكن إزالة النطاق العام",
            "Cannot pop global scope"
        );
    }
    
    // (AR) تحقق إضافي من حجم المكدس لمنع سلوك غير محدد
    // (EN) Additional stack size guard to prevent undefined behavior
    if (scopeStack_.size() <= 1) {
        throwError(
            "مكدس النطاقات فارغ — لا يمكن الإزالة",
            "Scope stack is empty — cannot pop"
        );
    }
    
    // Update current scope to parent
    currentScope_ = currentScope_->getParent();
    
    // Remove from stack
    scopeStack_.pop_back();
}

int ScopeManager::getCurrentDepth() const {
    return currentScope_->getDepth();
}

bool ScopeManager::isInFunction() const {
    Scope* scope = currentScope_;
    while (scope != nullptr) {
        if (scope->isFunction()) {
            return true;
        }
        scope = scope->getParent();
    }
    return false;
}

bool ScopeManager::isInLoop() const {
    Scope* scope = currentScope_;
    while (scope != nullptr) {
        if (scope->isLoop()) {
            return true;
        }
        scope = scope->getParent();
    }
    return false;
}

Scope* ScopeManager::findNearestScope(ScopeType type) const {
    Scope* scope = currentScope_;
    while (scope != nullptr) {
        if (scope->getType() == type) {
            return scope;
        }
        scope = scope->getParent();
    }
    return nullptr;
}

bool ScopeManager::isVariableDeclared(const std::string& name) const {
    Scope* scope = currentScope_;
    while (scope != nullptr) {
        if (scope->isVariableDeclared(name)) {
            return true;
        }
        scope = scope->getParent();
    }
    return false;
}

void ScopeManager::declareVariable(const std::string& name) {
    // Check if already declared in current scope (no shadowing in same scope)
    if (currentScope_->hasVariable(name)) {
        std::ostringstream oss;
        oss << "(AR) المتغير '" << name << "' معرّف مسبقاً في هذا النطاق "
            << "(EN) Variable '" << name << "' already declared in this scope";
        throw std::runtime_error(oss.str());
    }
    
    // Declare in current scope
    currentScope_->declareVariable(name);
}

Scope* ScopeManager::findVariableScope(const std::string& name) const {
    Scope* scope = currentScope_;
    while (scope != nullptr) {
        if (scope->hasVariable(name)) {
            return scope;
        }
        scope = scope->getParent();
    }
    return nullptr;
}

std::string ScopeManager::debugString() const {
    std::ostringstream oss;
    oss << "ScopeManager[";
    oss << "current=" << currentScope_->debugString();
    oss << ", depth=" << getCurrentDepth();
    oss << ", stack_size=" << scopeStack_.size();
    oss << "]";
    return oss.str();
}

void ScopeManager::printScopeStack() const {
    std::cout << "\n=== Scope Stack (النطاقات) ===\n";
    std::cout << "Total scopes: " << scopeStack_.size() << "\n";
    std::cout << "Current depth: " << getCurrentDepth() << "\n\n";
    
    for (size_t i = 0; i < scopeStack_.size(); i++) {
        const auto& scope = scopeStack_[i];
        std::cout << "  [" << i << "] ";
        if (scope.get() == currentScope_) {
            std::cout << "→ ";
        } else {
            std::cout << "  ";
        }
        std::cout << scope->debugString() << "\n";
    }
    std::cout << "=================================\n\n";
}

void ScopeManager::throwError(const std::string& messageAr, const std::string& messageEn) const {
    std::ostringstream oss;
    oss << "(AR) " << messageAr << " (EN) " << messageEn;
    throw std::runtime_error(oss.str());
}

} // namespace Data
} // namespace Sad
