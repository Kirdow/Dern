#include "pch.h"
#include "typeparser.h"

namespace Dern
{
    Ref<StoredValue> TypeParser::ComputeValue(std::function<Ref<Token>()> nextFn)
    {
        std::vector<Token> tokens;

        Ref<Token> token = nullptr;
        while ((token = nextFn()) != nullptr)
        {
            tokens.push_back(*token);
        }

        std::cout << "Tokens: " << tokens.size() << "\n";
        for (auto& token : tokens)
        {
            std::cout << token << " ";
        }
        std::cout << "\n";

        return CreateRef<StoredValue>();
    }
}