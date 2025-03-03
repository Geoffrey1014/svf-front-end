//
// Created by Weigang He on 3/3/2025.
//

#include "Ll.h"

std::string LlPhiStatement::toString() const{
    std::ostringstream ss;
    ss << *definedVar << " = phi [";
    for(size_t i = 0; i < incomingVars.size(); ++i) {
        if(i > 0) ss << ", ";
        ss << *incomingVars[i] << " from " << incomingBlocks[i]->getLabel();
    }
    ss << "]";
    return ss.str();
}