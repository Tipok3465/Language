#include "SemanticStack.h"

bool isLiteral(Lexeme lexeme) {
    return lexeme.getType() == LexemeType::FloatLiteral || lexeme.getType() == LexemeType::IntegerLiteral ||
           lexeme.getType() == LexemeType::BoolLiteral || lexeme.getType() == LexemeType::StringLiteral;
}

SemanticStack::SemanticStack(NameChecker &varChecker) : varChecker_(varChecker) {}

void SemanticStack::checkUno() {
    Lexeme operand = data_.back();
    data_.pop_back();
    Lexeme operation = data_.back();
    data_.pop_back();
    Lexeme res;
    if (isLiteral(operand)) {
        if (operand.toString() == "integer" || operand.toString() == "char" || operand.toString() == "float") {
            if (operation.getName() == "!") {
                res.setName("bool");
                res.setId(operand.getId());
                res.setType(LexemeType::BoolLiteral);
                push(res);
                return;
            }
        }
        throw Error("Incorrect type of unary operation: " + std::to_string(operation.getId()));
    }
    if (operation.getType() == LexemeType::Identifier) {
        if (operand.toString() != "integer" && operand.toString() != "char" && operand.toString() != "float") {
            throw Error("Incorrect type of unary operation: " + std::to_string(operation.getId()));
        }
        res.setName("integer");
        res.setId(operand.getId());
        res.setType(operand.getType());
        push(res);
        return;
    }
    throw Error("Incorrect type of unary operation: " + std::to_string(operation.getId()));
}

void SemanticStack::checkBin() {
    Lexeme rhs = data_.back();
    data_.pop_back();
    Lexeme op = data_.back();
    data_.pop_back();
    Lexeme lhs = data_.back();
    data_.pop_back();
    if (rhs.toString() != "integer" && rhs.toString() != "float" && rhs.toString() != "char" && rhs.toString() != "bool" &&
        rhs.toString() != "string" && rhs.toString() != "array") {
        throw Error("You can't do operations like this: " + std::to_string(rhs.getId()));
    }
    if (lhs.toString() != "integer" && lhs.toString() != "float" && lhs.toString() != "char" && lhs.toString() != "bool" &&
        lhs.toString() != "string" && lhs.toString() != "array") {
        throw Error("You can't do operations like this: " + std::to_string(rhs.getId()));
    }
    if (op.getName() == "integer" || op.getName() == "float" || op.getName() == "char" || op.getName() == "bool" ||
        op.getName() == "string" || op.getName() == "array") {
        throw Error("Expected operation, found operator: " + std::to_string(op.getId()));
    }
    Lexeme res;
    if (op.getName() == "-" || op.getName() == "*" || op.getName() == "/" ||
        op.getName() == "and" || op.getName() == "or") {
        if (lhs.toString() == "array" || rhs.toString() == "array") {
            throw Error("You can't do this operation with array: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == "string" || rhs.toString() == "string") {
            throw Error("You can't do this operation with string: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == "float" || lhs.toString() == "float") {
            res.setName("float");
            res.setId(rhs.getId());
            res.setType(LexemeType::FloatLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "integer" || lhs.toString() == "integer") {
            res.setName("integer");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "char" || lhs.toString() == "char") {
            res.setName("char");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "bool" || lhs.toString() == "bool") {
            res.setName("bool");
            res.setId(rhs.getId());
            res.setType(LexemeType::BoolLiteral);
            push(res);
            return;
        }
        throw Error("Something wrong in checkBin");
    }
    if (op.getName() == "+") {
        if (lhs.toString() == "array" || rhs.toString() == "array") {
            throw Error("You can't do this operation with array: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == "string" || rhs.toString() == "string") {
            if (lhs.toString() != rhs.toString()) {
                throw Error("You can't do this operation with string: " + std::to_string(op.getId()));
            }
            res.setName("string");
            res.setId(rhs.getId());
            res.setType(LexemeType::StringLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "float" || lhs.toString() == "float") {
            res.setName("float");
            res.setId(rhs.getId());
            res.setType(LexemeType::FloatLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "integer" || lhs.toString() == "integer") {
            res.setName("integer");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "char" || lhs.toString() == "char") {
            res.setName("char");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "bool" || lhs.toString() == "bool") {
            res.setName("bool");
            res.setId(rhs.getId());
            res.setType(LexemeType::BoolLiteral);
            push(res);
            return;
        }
        throw Error("Something wrong in checkBin");
    }
    if (op.getName() == "==" || op.getName() == "!=") {
        res.setName("bool");
        res.setId(rhs.getId());
        res.setType(LexemeType::BoolLiteral);
        push(res);
        if (lhs.toString() == "array" || rhs.toString() == "array") {
            throw Error("You can't do this operation with array: " + std::to_string(op.getId()));
        }
        if (rhs.toString() == "integer" || rhs.toString() == "float" || rhs.toString() == "char" || rhs.toString() == "bool") {
            if (lhs.toString() != "int" && lhs.toString() != "float" &&
                lhs.toString() != "char" && lhs.toString() != "bool") {
                throw Error("You can't do this operation with different types: " + std::to_string(op.getId()));
            }
            return;
        }
        if (lhs.toString() == "string") {
            if (rhs.toString() != "string") {
                throw Error("You can't comparison string with other types: " + std::to_string(op.getId()));
            }
            return;
        }
        throw Error("Something wrong in checkBin");
    }
    if (op.getName() == "<" || op.getName() == "<=" || op.getName() == ">=" || op.getName() == ">") {
        if (lhs.toString() == "array" || rhs.toString() == "array") {
            throw Error("You can't do this operation with array: " + std::to_string(op.getId()));
        }
        if ((rhs.toString() == "string" || lhs.toString() == "string") && rhs.toString() != lhs.toString()) {
            throw Error("You can't do this operation with string: " + std::to_string(op.getId()));
        }
        res.setName("bool");
        res.setId(rhs.getId());
        res.setType(LexemeType::BoolLiteral);
        push(res);
        return;
    }
    if (op.getName() == "%") {
        if (lhs.toString() == "array" || rhs.toString() == "array") {
            throw Error("You can't do this operation with array: " + std::to_string(op.getId()));
        }
        if (rhs.toString() == "string" || lhs.toString() == "string") {
            throw Error("You can't do this operation with string: " + std::to_string(op.getId()));
        }
        if (rhs.toString() == "float" || lhs.toString() == "float") {
            throw Error("You can't do this operation with float type: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == "integer" || rhs.toString() == "integer") {
            res.setName("integer");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "char" || rhs.toString() == "char") {
            res.setName("char");
            res.setId(rhs.getId());
            res.setType(LexemeType::IntegerLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "bool" || rhs.toString() == "bool") {
            res.setName("bool");
            res.setId(rhs.getId());
            res.setType(LexemeType::BoolLiteral);
            push(res);
            return;
        }
    }
    if (op.getName() == "[") {
        if (lhs.toString() != "array" && rhs.toString() != "string") {
            throw Error("You can't access the index: " + std::to_string(op.getId()));
        }
        if (rhs.toString() != "integer" && rhs.toString() != "char" && rhs.toString() != "bool") {
            throw Error("Wrong index address: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == "string") {
            res.setName("char");
            res.setId(rhs.getId());
            res.setType(LexemeType::StringLiteral);
            push(res);
            return;
        }
        if (lhs.toString() == "array") {
            std::string type_ = varChecker_.getType(lhs.getName()).substr(6);
            if (type_ == "integer") {
                res.setType(LexemeType::IntegerLiteral);
            } else if (type_ == "float") {
                res.setType(LexemeType::FloatLiteral);
            } else if (type_ == "char") {
                res.setType(LexemeType::IntegerLiteral);
            } else if (type_ == "bool") {
                res.setType(LexemeType::BoolLiteral);

            }
            res.setValue(true);
            res.setId(rhs.getId());
            push(res);
            return;
        }
    }
    if (op.getName() == "=") {
        if (!lhs.isLvalue()) {
            throw Error("You must do it with lvalue: " + std::to_string(op.getId()));
        }
        if (lhs.toString() == rhs.toString()) {
            res.setName(lhs.toString());
            res.setId(rhs.getId());
            res.setType(lhs.getType());
            push(res);
            return;
        }
        if (rhs.toString() == "integer" || rhs.toString() == "float" || rhs.toString() == "char" || rhs.toString() == "bool") {
            if (lhs.toString() != "integer" && lhs.toString() != "float" &&
                lhs.toString() != "char" && lhs.toString() != "bool") {
                throw Error("You can't assign these types: " + std::to_string(op.getId()));
            }
            res.setName(lhs.toString());
            res.setId(rhs.getId());
            res.setType(lhs.getType());
            res.setValue(true);
            push(res);
            return;
        }
        throw Error("You can't assign these types: " + std::to_string(op.getId()));
    }
    if (op.getName() == "return") {
        if (lhs.toString() != rhs.toString()) {
            throw Error("Wrong return type: " + std::to_string(op.getId()));
        }
    }
}

Lexeme SemanticStack::pop() {
    Lexeme res = data_.back();
    if (!data_.empty()) data_.pop_back();
    return res;
}

void SemanticStack::clear() {
    while (!data_.empty()) {
        data_.pop_back();
    }
}