using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace HoneCScore
{
    /// <sources>
    ///  See: https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/async/index
    ///       https://en.wikipedia.org/wiki/Carolina_Reaper
    ///       https://en.wikipedia.org/wiki/Reversible_cerebral_vasoconstriction_syndrome
    /// </sources>
    class Program
    {
        static readonly Func<int, Task<int>> λ = async (ξ) => 
        {
            Console.WriteLine($"A ϑ-breakfast: (|{ξ}| is in the making!");
            await Task.Delay(ξ * 1_000);
            return ξ;
        };
        // Jules: Hamburgers! The cornerstone of any nutritious breakfast.
        //        What kind of hamburgers?
        static async Task<int> Tastyϑ(int number)
        {
            var ϑ = await λ(number);
            return ϑ;
        }
        static async Task Main(string[] args)
        {
            var ϑs = new List<string> { "Jalapeño", "Cayenne", "Habanero" };
            var json = JsonSerializer.Serialize(ϑs);
            // JetiSON a request wherever you want, pull it back, process...
            // ... and spice-up with e.g. some in situ ingredients...
            var pepper = ("\"Carolina Reaper\"", "\"Scotch bonnet\"");

            ϑs = JsonSerializer.Deserialize<List<string>>(json);
            ϑs.AddRange(JsonSerializer.Deserialize<List<string>>($"[{pepper.Item1}, {pepper.Item2}]"));

            Console.OutputEncoding = Encoding.UTF8;
            Console.WriteLine($"A JSON's ϑ-breakfast toppings: {json}");
            foreach (var ϑ in ϑs)
                Console.WriteLine($"The re-JetiSONed ϑ-topping: {ϑ}");
            Console.WriteLine($"Preparing a ϑ-breakfast... at {DateTime.Now} with safety glasses and gloves");

            var ϑα = Tastyϑ(6); var ϑω = Tastyϑ(4);
            var allϑs = new List<Task> { ϑα, ϑω, Tastyϑ(5)};
            foreach (var ϑ in ϑs.Select(ω => Tastyϑ(ω.Length)))
                allϑs.Add(ϑ);

            int Noϑ = 1; var ρ = new Random();
            // See: https://www.youtube.com/watch?v=WqXgl8EZzcs for a 'WhenEach' pattern in C# .NET 9.0
            while (allϑs.Any())
            {
                Console.WriteLine($"The ϑ-burger №{Noϑ++} is being prepared");
                var ϑ = (Task<int>)await Task.WhenAny(allϑs);
                Console.WriteLine($"The ϑ-burger ready in just {ϑ.Result} seconds");
                allϑs.Remove(ϑ);

                // If you airily ask for a new burger... be careful what you wish for!
                if (ρ.Next(2) != ρ.Next(2))
                    allϑs.Add(λ(ρ.Next(2, 4)));
            }
            Console.WriteLine($"The multi-ϑ-burger breakfast is ready at {DateTime.Now}!");
            // Jules:     We happy? Vincent! We happy?"

            allϑs.Add(λ(3)); allϑs.Add(λ(2)); allϑs.Add(λ(4)); allϑs.Add(λ(9));
            await Task.WhenAll(allϑs);
            allϑs.Clear();
            // Butch:     So we cool?
            // Marsellus: Yeah, we cool. Two things. Don't tell nobody about this. [...]
            //            Two: you leave town tonight, right now.
            //            And when you're gone, you stay gone, or you be gone.
            //            You lost all your L.A. privileges.
            //            Deal ?
            Console.WriteLine($"The double-ϑ-burger makeweight is ready at {DateTime.Now}, too!");
        }
    }
}