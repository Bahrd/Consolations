/*Programming is one of the most difficult branches of applied mathematics; 
  the poorer mathematicians had better remain pure mathematicians." 
  Edsger W Dijkstra, "How do we tell truths that might hurt?", 1975
  */
import std.core;
int main()
{
    using std::views::iota, 
          std::views::filter, std::views::transform, 
          std::views::take;
    using std::placeholders::_1;
    // Where functions are the first class-citizens...: 
    // https://en.wikipedia.org/wiki/First-class_function    
    // A tuple of a(uto)nonymous [a.k.a. template-in-auto-disguise] items... 
    auto lambdas = std::tuple([](auto i) -> bool        { return !(i & 0b1); },
                              [](auto i)                { return i ? static_cast<double>(01) / i : 0.0; },
                              [](auto i, char e = '\t') { return std::cout << std::format("{:.{}g}{}", i, 0b11, e), i; });
    
    // Named tuple/pair...  with an explicit template 
    // anonymous function declaration
    auto [twice, writeline] = std::pair([]<typename T>(T i) { return 0b10 * i; },
                                        std::bind(std::get<0b10>(lambdas), _1, '\n'));
    
    auto twicer = std::ranges::istream_view<double>(std::cin) | transform(twice);
    std::ranges::copy(twicer, std::ostream_iterator<double>(std::cout, "\n"));
    // Not ready for a prime time yet: 
    // 'std::ranges::ostream_iterator<double>(std::cout, "\n")'
    // See e.g.: http://ericniebler.github.io/range-v3/index.html and https://cxx20ranges.brcha.com/#/5/19
    // A bit of command line magic:
    // https://devblogs.microsoft.com/cppblog/using-cpp-modules-in-msvc-from-the-command-line-part-1/
    // Compose a range...
    auto lambada = iota(-11)                         | 
                   filter(std::get<0b0>(lambdas))    | 
                   transform(std::get<0b1>(lambdas)) | 
                   transform(std::get<0b10>(lambdas))| 
                   take(11);
    // ... and execute it 
    //     (somehow)!
    for(auto _ : lambada);
    return 0;
}