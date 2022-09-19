#include "pch.h"
#include "program.h"
#include "tokenizer.h"

namespace Dern
{
	LProgram::LProgram(const std::string& filePath)
    {
        m_Code = CreateScope<Code>(filePath);
    }

    LProgram::~LProgram()
    {
    }

    void LProgram::Run()
    {
        auto tokenizer = CreateScope<Tokenizer>(m_Code);
        if (!tokenizer->Tokenize())
        {
            std::cerr << "Failed to tokenize code!\n";
            return;
        }

        Scope<TokenData> data = tokenizer->GetTokenData();
        int instructions = 0;
        int itokens = 0;
        int tokens = 0;
        for (int i = 0; i < data->Count(); i++)
        {
            if (itokens == 0)
                std::cout << "Instruction " << instructions << ": ";

            auto& token = data->At(i);
            if (token->GetType() == TokenType::Sym && token->GetData() == ";")
            {
                std::cout << "\n";
                itokens = 0;
                ++instructions;
                continue;
            }

            if (itokens > 0) std::cout << " ";
            std::cout << *token;
            ++itokens;
            ++tokens;
        }

        std::cout << "Totals:\n";
        std::cout << "  Instructions: " << instructions << "\n";
        std::cout << "  Tokens: " << tokens << "\n";
    }
}

