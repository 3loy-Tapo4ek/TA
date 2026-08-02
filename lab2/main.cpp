#include "CoolRegex.hpp"
#include <iostream>

int main()
{
    // Ищем вызовы функций get(...) и set(...) с аргументами из a, b, 0, 1
    CoolRegex re("(get|set)%(%(a|b|0|1)...%)%;");
    re.compile();

    std::string code = R"(
        void process() {
            int x = get(a01);
            set(b100);
            
            // Это не должно совпасть (другие буквы):
            get(xyz99); 
            
            if (x > 0) {
                get(); // Пустые скобки - должно совпасть!
                reset(a01); // Префикс "reset" не подходит
            }
            
            // Сложный вызов с длинными аргументами:
            set(a0101b101a);
            
            /* Без точки с запятой в конце - не должно совпасть */
            get(a1)
        }
    )";

    // Можно также визуализировать весь путь компиляции этой сложной регулярки:
    re.visualizePipeline("complex_test");

    auto matches = re.findAll(code);

    std::cout << "=== НАЙДЕННЫЕ ВЫЗОВЫ ФУНКЦИЙ ===" << std::endl;
    std::cout << "Всего совпадений: " << matches.size() << std::endl;
    for (size_t i = 0; i < matches.size(); ++i) {
        std::cout << i + 1 << ". " << matches[i] << std::endl;
    }

    return 0;
}