import Foundation

public enum ConsoleInput {
    public static func read<T>(
        prompt: String,
        reader: () -> String? = { readLine() },
        parse: (String) throws -> T
    ) -> T? {
        while true {
            print(prompt, terminator: " ")
            guard let line = reader() else { return nil }
            do {
                return try parse(line)
            } catch {
                print("Ошибка: \(error.localizedDescription) Попробуйте ещё раз.")
            }
        }
    }
}
