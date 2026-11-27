import Foundation

/// Реализация консольного ввода-вывода, соответствующая `ConsoleInputProtocol`.
public final class ConsoleInput: ConsoleInputProtocol {
    private let reader: () -> String?
    private let writer: (String) -> Void
    private let promptWriter: (String) -> Void

    public init(
        reader: @escaping () -> String? = { Swift.readLine() },
        writer: @escaping (String) -> Void = { print($0) },
        promptWriter: @escaping (String) -> Void = { print($0, terminator: " ") }
    ) {
        self.reader = reader
        self.writer = writer
        self.promptWriter = promptWriter
    }

    public func readLine(prompt: String) -> String? {
        promptWriter(prompt)
        return reader()
    }

    public func readValidated<T>(prompt: String, parse: (String) throws -> T) -> T? {
        while true {
            promptWriter(prompt)
            guard let line = reader() else {
                return nil
            }
            do {
                return try parse(line)
            } catch {
                writer("Ошибка: \(error.localizedDescription) Попробуйте ещё раз.")
            }
        }
    }

    public func write(_ message: String) {
        writer(message)
    }
}
