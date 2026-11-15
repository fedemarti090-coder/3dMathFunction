#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <charconv>
#include <cmath>
#include <stdexcept>

#include <fstream> // per debug

#include "mathLib.hpp"

class MathEngine
{
private:
    float DominioMax = 3.14f *  2;
    float DominioMin = -3.14f * 2;
    float step = 0.1f;

    int stepsPerAxis = 0;
    int verticesPerAxis = 0;

    // -------- DEBUG --------
    bool debug = false;
    bool rawData = false;
    bool outDebug = false;
    // -------- ----- --------

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

    bool isFunction(const std::string &str)
    {
        return str == "sin" || str == "cos" || str == "tan";
    }

    bool isAlphaString(const std::string &str)
    {
        if (str.empty())
            return false;
        return std::isalpha(static_cast<unsigned char>(str[0])) && str != "x" && str != "y";
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
                    if (isStringFloat(last) || last == "x" || last == "y" || last == ")" || (isAlphaString(last) && !isFunction(last)))
                    {
                        fTokenized.push_back("*");
                    }
                }

                fTokenized.push_back(numStr);
                continue;
            }

            if (std::isalpha(c))
            {
                std::string alphaStr = "";
                while (i < f.length() && std::isalpha(f[i]))
                {
                    alphaStr += f[i];
                    i++;
                }

                std::string functionName;
                std::string shorthandArgument;
                if (alphaStr.size() == 4 && isFunction(alphaStr.substr(0, 3)) &&
                    (alphaStr[3] == 'x' || alphaStr[3] == 'y'))
                {
                    functionName = alphaStr.substr(0, 3);
                    shorthandArgument = alphaStr.substr(3, 1);
                }

                if (!fTokenized.empty())
                {
                    std::string last = fTokenized.back();
                    if (isStringFloat(last) || last == "x" || last == "y" || last == ")" || (isAlphaString(last) && !isFunction(last)))
                    {
                        fTokenized.push_back("*");
                    }
                }

                if (!functionName.empty())
                {
                    fTokenized.push_back(functionName);
                    fTokenized.push_back(shorthandArgument);
                }
                else
                {
                    fTokenized.push_back(alphaStr);
                }
                continue;
            }

            std::string operators = "+-*/^()";
            if (operators.find(c) != std::string::npos)
            {
                std::string tokenStr = std::string(1, c);

                if (!fTokenized.empty())
                {
                    std::string last = fTokenized.back();

                    bool lastIsNum = isStringFloat(last);
                    bool lastIsVar = (last == "x" || last == "y");
                    bool lastIsCloseParen = (last == ")");
                    bool lastIsAlpha = isAlphaString(last) && !isFunction(last);

                    bool currentIsVar = (tokenStr == "x" || tokenStr == "y");
                    bool currentIsOpenParen = (tokenStr == "(");

                    if ((lastIsNum || lastIsVar || lastIsCloseParen || lastIsAlpha) && (currentIsVar || currentIsOpenParen))
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
        if (op == "sin" || op == "cos" || op == "tan")
            return 4;
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

            if (isAlphaString(chunk))
            {
                operatorStack.push_back(chunk);
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

                if (!operatorStack.empty() && isAlphaString(operatorStack.back()))
                {
                    outputQueue.push_back(operatorStack.back());
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
                    throw std::runtime_error("Espressione non valida: operatore binario senza due operandi.");
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
            else if (token == "sin" || token == "cos" || token == "tan")
            {
                if (evalStack.empty())
                {
                    throw std::runtime_error("Espressione non valida: funzione trigonometrica senza argomento.");
                }

                float a = evalStack.back();
                evalStack.pop_back();

                float result = 0.0f;
                if (token == "sin")
                    result = std::sin(a);
                else if (token == "cos")
                    result = std::cos(a);
                else if (token == "tan")
                    result = std::tan(a);

                evalStack.push_back(result);
            }
            else
            {
                throw std::runtime_error("Token non riconosciuto nell'espressione: " + token);
            }
        }

        if (evalStack.size() != 1)
        {
            throw std::runtime_error("Espressione non valida: deve produrre un solo risultato.");
        }

        return evalStack.back();
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

    void outputCSVtoDebug(std::vector<Vertex> list, std::string fileName)
    {

        std::ofstream file(fileName);

        if (file.is_open())
        {
            file << "x, " << "y, " << "z, " << "\n";
            for (const Vertex &r : list)
            {
                file << r.x << ", " << r.y << ", " << r.z << "\n";
            }
            file.close();
        }
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

        if (outDebug)
        {
            outputCSVtoDebug(vertexList, "../data/outputDebug.csv");
        }

        return true;
    }

    std::vector<Vertex> getVertexList() { return vertexList; }
    std::vector<uint32_t> getIndices() { return indices; }
};