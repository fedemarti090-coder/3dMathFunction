#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <charconv>
#include <cmath>

#include "mathLib.hpp"

class MathEngine
{
private:
    float DominioMax = 1.0f;
    float DominioMin = -1.0f;
    float step = 0.1f;

    int stepsPerAxis = 0;
    int verticesPerAxis = 0;

    bool debug = false;
    bool rawData = true;

    std::vector<Vertex> vertexList = {};
    std::vector<uint32_t> indices = {};

    // metodo -> shunting-Yard

    bool isNumber(char carattere)
    {
        std::vector<char> Number = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};

        auto isNumber = std::find(Number.begin(), Number.end(), carattere);

        return isNumber != Number.end();
    }

    bool isOperator(char carattere)
    {
        std::vector<char> Operator = {'+', '-', '*', '/', '^'};

        auto isoperator = std::find(Operator.begin(), Operator.end(), carattere);

        return isoperator != Operator.end();
    }

    std::vector<std::string> tokenizeFunction(std::string f)
    {
        std::vector<std::string> fTokenized;
        int i = 0;

        while (i < f.length())
        {
            char c = f[i];

            if (std::isspace(c))
            {
                i++;
                continue;
            }

            if (isNumber(c))
            {
                std::string numStr = "";
                while (i < f.length() && isNumber(f[i]))
                {
                    numStr += f[i];
                    i++;
                }

                if (!fTokenized.empty())
                {
                    std::string last = fTokenized.back();
                    if (last == "x" || last == "y" || last == ")")
                    {
                        fTokenized.push_back("*");
                    }
                }

                fTokenized.push_back(numStr);
                continue;
            }

            std::string operators = "+-*/^()";
            if (operators.find(c) != std::string::npos || c == 'x' || c == 'y')
            {
                std::string tokenStr = std::string(1, c);

                if (!fTokenized.empty())
                {
                    std::string last = fTokenized.back();

                    bool lastIsNum = isStringFloat(last);
                    bool lastIsVar = (last == "x" || last == "y");
                    bool lastIsCloseParen = (last == ")");

                    bool currentIsVar = (tokenStr == "x" || tokenStr == "y");
                    bool currentIsOpenParen = (tokenStr == "(");

                    if ((lastIsNum || lastIsVar || lastIsCloseParen) && (currentIsVar || currentIsOpenParen))
                    {
                        fTokenized.push_back("*");
                    }
                }

                fTokenized.push_back(tokenStr);
                i++;
                continue;
            }

            i++;
        }

        return fTokenized;
    }

    bool stampaFuncStr(std::vector<std::string> f)
    {
        for (const std::string &token : f)
        {
            std::cout << token << "  ";
        }
        std::cout << std::endl;

        return true;
    }

    bool stampaVertexList(std::vector<Vertex> list)
    {
        std::cout << list.size() << "Vertex list:\n";
        for (const Vertex &point : list)
        {
            if (rawData)
            {
                std::cout << point.x << ",  " << point.y << ", " << point.z << ", ";
            }
            else
            {
                std::cout << "< " << point.x << ",  " << point.y << ", " << point.z << " >\n";
            }
        }
        std::cout << std::endl;

        return true;
    }

    bool stampaIndices(std::vector<uint32_t> list)
    {
        std::cout << list.size() << "Indice list:\n";
        for (int i = 0; i < list.size(); i += 3)
        {
            if (i + 3 > list.size())
                break;
            std::cout << "< " << list[i] << " " << list[i + 1] << " " << list[i + 2] << " >\n";

            if (i > 20)
                return true;
        }
        std::cout << std::endl;

        return true;
    }

    bool isStringFloat(const std::string &chunk)
    {
        float val = 0.0f;
        auto [ptr, ec] = std::from_chars(chunk.data(), chunk.data() + chunk.size(), val);

        return ec == std::errc() && ptr == chunk.data() + chunk.size();
    }

    int getPrecedence(const std::string &op)
    {
        if (op == "+" || op == "-")
            return 1;
        if (op == "*" || op == "/")
            return 2;
        if (op == "^")
            return 3;
        return 0;
    }

    std::vector<std::string> shuntingYard(std::vector<std::string> tokenizedFunction)
    {
        std::vector<std::string> outputQueue;
        std::vector<std::string> operatorStack;

        for (const std::string &chunk : tokenizedFunction)
        {
            if (isStringFloat(chunk) || chunk == "x" || chunk == "y")
            {
                outputQueue.push_back(chunk);
                continue;
            }

            if (chunk == "(")
            {
                operatorStack.push_back(chunk);
                continue;
            }

            if (chunk == ")")
            {
                while (!operatorStack.empty() && operatorStack.back() != "(")
                {
                    outputQueue.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                if (!operatorStack.empty() && operatorStack.back() == "(")
                {
                    operatorStack.pop_back();
                }
                continue;
            }

            if (chunk == "+" || chunk == "-" || chunk == "*" || chunk == "/" || chunk == "^")
            {
                while (!operatorStack.empty() && operatorStack.back() != "(")
                {
                    std::string topOp = operatorStack.back();

                    bool currentIsRightAssoc = (chunk == "^");
                    int currentPrec = getPrecedence(chunk);
                    int topPrec = getPrecedence(topOp);

                    if ((!currentIsRightAssoc && currentPrec <= topPrec) ||
                        (currentIsRightAssoc && currentPrec < topPrec))
                    {
                        outputQueue.push_back(topOp);
                        operatorStack.pop_back();
                    }
                    else
                    {
                        break;
                    }
                }
                operatorStack.push_back(chunk);
                continue;
            }
        }

        while (!operatorStack.empty())
        {
            outputQueue.push_back(operatorStack.back());
            operatorStack.pop_back();
        }

        return outputQueue;
    }

    float calcPoint(float x, float y, std::vector<std::string> rpnQueue)
    {
        std::vector<float> evalStack;

        for (const std::string &token : rpnQueue)
        {
            if (isStringFloat(token))
            {
                evalStack.push_back(std::stof(token));
            }

            else if (token == "x")
            {
                evalStack.push_back(x);
            }
            else if (token == "y")
            {
                evalStack.push_back(y);
            }

            else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^")
            {
                if (evalStack.size() < 2)
                {
                    return 0.0f;
                }

                float b = evalStack.back();
                evalStack.pop_back();
                float a = evalStack.back();
                evalStack.pop_back();

                float result = 0.0f;

                if (token == "+")
                    result = a + b;
                else if (token == "-")
                    result = a - b;
                else if (token == "*")
                    result = a * b;
                else if (token == "/")
                    result = (b != 0.0f) ? (a / b) : 0.0f;
                else if (token == "^")
                    result = std::pow(a, b);

                evalStack.push_back(result);
            }
        }

        if (!evalStack.empty())
        {
            return evalStack.back();
        }

        return 0.0f;
    }

    std::vector<Vertex> avviaCalcolo(std::vector<std::string> f)
    {
        std::vector<Vertex> vList;

        stepsPerAxis = std::round((DominioMax - DominioMin) / step);
        verticesPerAxis = stepsPerAxis + 1;

        for (int xi = 0; xi <= stepsPerAxis; ++xi)
        {
            float x = DominioMin + xi * step;
            for (int yi = 0; yi <= stepsPerAxis; ++yi)
            {
                float y = DominioMin + yi * step;
                float z = calcPoint(x, y, f);

                vList.push_back({x, y, z});
            }
        }
        return vList;
    }

    std::vector<uint32_t> generateIndices()
    {
        std::vector<uint32_t> ind;

        for (int i = 0; i < stepsPerAxis; i++)
        {
            for (int j = 0; j < stepsPerAxis; j++)
            {
                uint32_t current = i * verticesPerAxis + j;
                uint32_t right = i * verticesPerAxis + (j + 1);
                uint32_t bottom = (i + 1) * verticesPerAxis + j;
                uint32_t botRight = (i + 1) * verticesPerAxis + (j + 1);

                // Primo triangolo
                ind.push_back(current);
                ind.push_back(bottom);
                ind.push_back(right);

                // Secondo triangolo
                ind.push_back(right);
                ind.push_back(bottom);
                ind.push_back(botRight);
            }
        }
        return ind;
    }

public:
    bool calcolaFunzione3D(std::string f)
    {
        std::vector<std::string> Ans = tokenizeFunction(f);
        std::vector<std::string> AnsSY = shuntingYard(Ans);

        vertexList = avviaCalcolo(AnsSY);
        indices = generateIndices();

        if (debug)
        {
            stampaFuncStr(Ans);
            stampaFuncStr(AnsSY);
            stampaVertexList(vertexList);
            stampaIndices(indices);
        }

        return true;
    }

    std::vector<Vertex> getVertexList() { return vertexList; }
    std::vector<uint32_t> getIndices() { return indices; }
};