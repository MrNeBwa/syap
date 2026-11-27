import Foundation

/// Протокол валидатора пользовательского ввода.
/// Определяет единый контракт проверки пунктов меню и числовых значений разных типов.
public protocol InputValidatorProtocol: AnyObject {
    /// Проверяет корректность выбора пункта меню в заданном диапазоне.
    func validateMenuChoice(_ raw: String, in range: ClosedRange<Int>) throws -> Int
    /// Проверяет и преобразует строку в целое число `Int`.
    func validateInt(_ raw: String) throws -> Int
    /// Проверяет и преобразует строку в вещественное число двойной точности `Double`.
    func validateDouble(_ raw: String) throws -> Double
    /// Проверяет и преобразует строку в вещественное число одинарной точности `Float`.
    func validateFloat(_ raw: String) throws -> Float
    /// Проверяет и преобразует строку в десятичное число высокой точности `Decimal`.
    func validateDecimal(_ raw: String) throws -> Decimal
}
