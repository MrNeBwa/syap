import Foundation

/// Протокол взаимодействия с консолью (ввод и вывод данных).
public protocol ConsoleInputProtocol: AnyObject {
    /// Выводит приглашение и считывает одну строку ввода.
    func readLine(prompt: String) -> String?
    /// Повторяет запрос ввода до тех пор, пока переданная функция разбора не вернёт корректное значение или поток ввода не завершится.
    func readValidated<T>(prompt: String, parse: (String) throws -> T) -> T?
    /// Выводит сообщение в консоль.
    func write(_ message: String)
}
