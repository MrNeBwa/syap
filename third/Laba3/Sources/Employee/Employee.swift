import InputValidation
public final class Employee {
    var name: String
    var jobTitle: String
    var experience: Int

    /// Создаёт сотрудника. Выбрасывает `ValidationError`, если данные некорректны.
    public init(name: String, jobTitle: String, experience: Int) throws {
        self.name = try Validator.validateName(name)
        self.jobTitle = try Validator.validateJobTitle(jobTitle)
        self.experience = try Validator.validateExperience(experience)
    }

    public func updateJobTitle(to newJobTitle: String) throws {
        self.jobTitle = try Validator.validateJobTitle(newJobTitle)
    }
    
    /// Строка вида «Имя: Василий Должность: Системный администратор Стаж: 3 года».
    public var info: String {
        "Имя: \(name) Должность: \(jobTitle) Стаж: \(YearsFormatter.format(experience))"
    }

    /// Выводит информацию о сотруднике в консоль.
    public func printInfo() {
        print(info)
    }
}
