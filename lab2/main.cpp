#include "CoolRegex.hpp"
#include "FAOperator.hpp"
#include <iostream>

int main()
{
    try 
    {
        CoolRegex re("(a{2}|b)...");
        re.compile();

        re.visualizePipeline("Before");

        std::string origin_expression = FAOperator::ReconstructRegex(re.getDFA());

        CoolRegex new_regex(origin_expression);
        new_regex.compile();
        new_regex.visualizePipeline("After");

        std::cout << origin_expression << std::endl;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << " Ошибка: " << e.what() << std::endl;
    }

    return 0;
}