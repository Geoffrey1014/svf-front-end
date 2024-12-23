// Ll.cpp
#include "Ll.h"


std::string LlComponent::toString() {
    return "LlComponent";
}

bool LlComponent::operator==(const LlComponent& other) const {
    // Implement comparison logic
    return true;
}

std::size_t LlComponent::hashCode() const {
    // Implement hash code logic
    return 0;
}



const std::string* LlLocation::getVarName() const {
    return varName;
}

bool LlLocation::operator==(const LlLocation& other) const {
    return *varName == *other.varName;
}

std::size_t LlLocation::hashCode() const {
    return std::hash<std::string>()(*varName);
}