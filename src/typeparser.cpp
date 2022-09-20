#include "pch.h"
#include "typeparser.h"

namespace Dern
{
    Ref<StoredValue> TypeParser::ComputeValue(std::function<Ref<Token>(ParseMem&)> nextFn)
    {
        std::vector<Token> tokens;

        Ref<Token> token = nullptr;
        while ((token = nextFn(m_Mem)) != nullptr)
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