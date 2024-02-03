/*Programming is one of the most difficult branches of applied mathematics; 
  the poorer mathematicians had better remain pure mathematicians." 
  Edsger W. Dijkstra, "How do we tell truths that might hurt?", 1975
  */
import std.core;
import std.threading;

int main()
{
    using namespace std::views;
    using std::placeholders::_1, std::bind;
    using std::string, std::vformat, std::make_format_args,
          std::cin, std::cout, 
          std::get, std::tuple;
    // Where functions are the first class-citizens...: 
    // https://en.wikipedia.org/wiki/First-class_function    
     // A tuple of a(uto)nonymous [a.k.a. template-in-auto-disguise] items...
    auto lambdas = tuple([](auto i) -> bool                { return !(i & 0b1); },
                         [](auto i)                        { return i ? static_cast<double>(01) / i : 0.0; },
                         [](auto i, string f = "{:.3g}")   { return vformat(f, make_format_args(i)); });
    // Named tuple/pair...  with explicit template 
    // anonymous function declarations (note the famous all-in-one brackets '[]<>(){}' syntax
    auto [twice, formatter, panta_rhei] = tuple([]<typename T>(T i) { return 0b10 * i; },
                                                bind(get<0b10>(lambdas), _1, "{}\n"),
                                                []<typename T>(T s) { return cout << s, s; });
    // https://youtu.be/j9tlJAqMV7U?t=469 - a composite pipeline that can reduce to a single assembler code line
    auto twicer = std::ranges::istream_view<double>(cin) |
                                        transform(twice) | 
                                    transform(formatter) | 
                                    transform(panta_rhei);
    for (auto _ : twicer);
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
    // ... "you cannot step twice into the same stream"...
    std::ranges::copy(lambada, std::ostream_iterator<string>(cout, "\t"));

    // See https://en.cppreference.com/w/cpp/string/basic_string_view 
    // & use: 'chcp 65001' in Windows Terminal/PowerShell
    constexpr std::string_view illusion[]    { "▀▄─", "▄▀─", "▀─▄", "▄─▀" };
    for (auto y = (cout << '\n', 0); y != (0b10 << 0b10); ++y, cout << '\n')
        for (auto x = 0; x != 0b10 << 0b10; ++x, cout << illusion[y % std::ssize(illusion)]);
    

    return 0;
}