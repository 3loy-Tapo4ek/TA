#include "CoolRegex.hpp"
#include "FAOperator.hpp"
#include "DotVisualizer.hpp"
#include <iostream>

int main()
{
    try 
    {
        // 1. Создаем выражения L1 = {"a", "b", "c"} и L2 = {"b"}
        auto re1 = CoolRegex::compile("a|b|c");
        auto re2 = CoolRegex::compile("b");

        // 2. Строим разность L1 \ L2
        FA diff_fa = FAOperator::MakeDifference(re1.getDFA(), re2.getDFA());
        CoolRegex diff_re(std::move(diff_fa));

        // 3. Сохраняем граф результата для проверки
        diff_re.visualizePipeline("Diff");
    } 
    catch (const std::exception& e) 
    {
        std::cerr << " Ошибка: " << e.what() << std::endl;
    }

    return 0;
}