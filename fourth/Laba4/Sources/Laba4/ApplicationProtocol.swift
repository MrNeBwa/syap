import Foundation

/// Протокол точки запуска интерактивного приложения.
public protocol ApplicationProtocol: AnyObject {
    func run()
}
