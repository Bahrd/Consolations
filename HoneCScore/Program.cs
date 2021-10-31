using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace HoneCScore
{
    /// <summary>
    ///  See: https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/async/index
    /// </summary>
    class Program
    {
        static Func<int, Task<int>> λ = async (ξ)
            => {
                Console.WriteLine($"A ϑ-breakfast recipe: {ξ} in the making!");
                await Task.Delay(ξ * 1_000);
                return ξ;
            };
        static async Task<int> Tastyϑ(int number)
        {
            var ϑ = await λ(number);
            return ϑ;
        }
        static async Task Main(string[] args)
        {
            // Inter-JSON-iser...
            var ϑs = new List<int> { -1, 0, +1 };
            var json = JsonSerializer.Serialize(ϑs);
            ϑs = JsonSerializer.Deserialize<List<int>>(json);

            ϑs = JsonSerializer.Deserialize<List<int>>("[ 1, 2, 3 ]");

            Console.OutputEncoding = Encoding.UTF8;
            Console.WriteLine($"A JSON's ϑ-breakfast recipe: {json}");
            foreach (var ϑ in ϑs.Select(ω => -1 * ω))
                Console.WriteLine($"A JSON's restored ϑ-value: {ϑ}");
            Console.WriteLine($"Preparing a ϑ-breakfast... at {DateTime.Now}");

            var ϑα = Tastyϑ(6); var ϑω = Tastyϑ(4);
            var allϑs = new List<Task> { ϑα, ϑω, Tastyϑ(5), Tastyϑ(5), Tastyϑ(7) };
            int Noϑ = 1; var ρ = new Random();
            while (allϑs.Any())
            {
                Console.WriteLine($"The ϑ-toast №{Noϑ++} is being prepared");
                var ϑ = (Task<int>)await Task.WhenAny(allϑs);
                Console.WriteLine($"The ϑ-toast ready in just {ϑ.Result} seconds");
                allϑs.Remove(ϑ);

                // Add randomly a new toast...
                //if (ρ.Next(2) != ρ.Next(2))
                //    allϑs.Add(λ(ρ.Next(2, 4)));
            }
            Console.WriteLine($"The multi-ϑ-toast breakfast is ready at {DateTime.Now}!");
            allϑs.Add(λ(3)); allϑs.Add(λ(2)); allϑs.Add(λ(4)); allϑs.Add(λ(9));
            await Task.WhenAll(allϑs);
            allϑs.Clear();
            Console.WriteLine($"The double-ϑ-toast makeweight is ready at {DateTime.Now}, too!");
        }
    }
}