#pragma once

#include <memory>
#include <istream>
#include "ConsoleView.hpp"
#include "IRecognizer.hpp"

#include "RegexRecognizer.hpp"
#include "FlexRecognizer.hpp"
#include "SMCRecognizer.hpp"
#include "Benchmark.hpp"
#include <fstream>

class AppController
{
private:
    ConsoleView& view_;
    std::unique_ptr<IRecognizer> recognizer_;

    //helpers
    bool setupEngine();
    void runInteractiveMode();
    void runFileMode();
    void runBenchmarkMode();
    
    //Common processor for data_stream
    void processStream(std::istream& in, bool is_interactive);

    //Fabric
    std::unique_ptr<IRecognizer> createRecognizer(EngineType choice) const;
public:
    explicit AppController(ConsoleView& view);
    void run();

};