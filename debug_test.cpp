#include <iostream>
#include <vector>
#include <string>

// 简单的调试测试程序
class DebugTest {
private:
    std::vector<int> numbers;
    std::string name;

public:
    DebugTest(const std::string& n) : name(n) {
        // 在这里设置断点测试
        std::cout << "创建DebugTest对象: " << name << std::endl;
    }

    void addNumber(int num) {
        numbers.push_back(num);
        std::cout << "添加数字: " << num << std::endl;
    }

    void printNumbers() {
        std::cout << "数字列表 (" << name << "):" << std::endl;
        for (size_t i = 0; i < numbers.size(); ++i) {
            // 在这里设置断点查看变量
            std::cout << "  [" << i << "] = " << numbers[i] << std::endl;
        }
    }

    int calculateSum() {
        int sum = 0;
        for (int num : numbers) {
            sum += num;  // 在这里设置断点
        }
        return sum;
    }
};

int main() {
    std::cout << "=== 麒麟系统调试测试程序 ===" << std::endl;
    
    // 创建测试对象
    DebugTest test("调试测试");
    
    // 添加一些数字
    test.addNumber(10);
    test.addNumber(20);
    test.addNumber(30);
    test.addNumber(40);
    test.addNumber(50);
    
    // 打印数字
    test.printNumbers();
    
    // 计算总和
    int sum = test.calculateSum();
    std::cout << "总和: " << sum << std::endl;
    
    std::cout << "=== 调试测试完成 ===" << std::endl;
    return 0;
}
