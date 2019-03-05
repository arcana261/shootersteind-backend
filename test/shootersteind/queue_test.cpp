#include <shootersteind/queue.h>
#include <string>
#include <thread>
#include <testing.h>

void test_emptyqueue_should_have_length_zero() {
    shooterstein::Queue<int> x;
    T_CMPINT(x.size(), ==, 0);
}

void test_emptyqueue_should_be_empty() {
    shooterstein::Queue<int> x;
    T_TRUE(x.empty());
}

void test_add_int_should_increase_size() {
    shooterstein::Queue<int> x;
    x.enqueue(1);
    T_CMPINT(x.size(), ==, 1);
}

void test_add_int_should_declare_not_empty() {
    shooterstein::Queue<int> x;
    x.enqueue(1);
    T_FALSE(x.empty());
}

void test_add_int_should_be_dequeueable() {
    shooterstein::Queue<int> x;
    x.enqueue(1);
    T_CMPINT(x.dequeue(), ==, 1);
}

void test_dequeue_int_should_decrease_size() {
    shooterstein::Queue<int> x;
    x.enqueue(1);
    x.dequeue();
    T_CMPINT(x.size(), ==, 0);
}

void test_dequeue_int_should_set_empty() {
    shooterstein::Queue<int> x;
    x.enqueue(1);
    x.dequeue();
    T_TRUE(x.empty());
}

void test_should_enqueue_copy_string() {
    shooterstein::Queue<std::string> x;
    std::string str("hello");
    x.enqueue(str);
}

void test_should_enqueue_move_string() {
    shooterstein::Queue<std::string> x;
    std::string str("hello");
    x.enqueue(std::move(str));
    T_TRUE(str.empty());
}

void test_should_dequeue_string() {
    shooterstein::Queue<std::string> x;
    x.enqueue(std::string("hello"));
    T_EQUAL_STR(x.dequeue().c_str(), "hello");
}

void test_enqueue_dequeue_some_ints() {
    shooterstein::Queue<int> x;

    for (int i = 1; i <= 4; i++) {
        x.enqueue(i);
    }

    T_CMPINT(x.dequeue(), ==, 1);
    T_CMPINT(x.dequeue(), ==, 2);

    for (int i = 5; i <= 12; i++) {
        x.enqueue(i);
    }

    T_CMPINT(x.dequeue(), ==, 3);
    T_CMPINT(x.dequeue(), ==, 4);
    T_CMPINT(x.dequeue(), ==, 5);
    T_CMPINT(x.dequeue(), ==, 6);
    T_CMPINT(x.dequeue(), ==, 7);
    T_CMPINT(x.dequeue(), ==, 8);
    T_CMPINT(x.dequeue(), ==, 9);
    T_CMPINT(x.dequeue(), ==, 10);

    for (int i = 13; i <= 40; i++) {
        x.enqueue(i);
    }

    T_CMPINT(x.dequeue(), ==, 11);
    T_CMPINT(x.dequeue(), ==, 12);

    for (int i = 13; i <= 40; i++) {
        T_CMPINT(x.dequeue(), ==, i);
    }

    for (int i = 41; i <= 100; i++) {
        x.enqueue(i);
    }
}

void test_enqueue_four_strings() {
    shooterstein::Queue<std::string> x;
    x.enqueue("1");
    x.enqueue("2");
    x.enqueue("3");
    x.enqueue("4");
}

void test_enqueue_dequeue_some_strings() {
    shooterstein::Queue<std::string> x;

    for (int i = 1; i <= 4; i++) {
        x.enqueue(std::to_string(i));
    }

    T_EQUAL_STR(x.dequeue().c_str(), "1");
    T_EQUAL_STR(x.dequeue().c_str(), "2");

    for (int i = 5; i <= 12; i++) {
        x.enqueue(std::to_string(i));
    }

    T_EQUAL_STR(x.dequeue().c_str(), "3");
    T_EQUAL_STR(x.dequeue().c_str(), "4");
    T_EQUAL_STR(x.dequeue().c_str(), "5");
    T_EQUAL_STR(x.dequeue().c_str(), "6");
    T_EQUAL_STR(x.dequeue().c_str(), "7");
    T_EQUAL_STR(x.dequeue().c_str(), "8");
    T_EQUAL_STR(x.dequeue().c_str(), "9");
    T_EQUAL_STR(x.dequeue().c_str(), "10");

    for (int i = 13; i <= 40; i++) {
        x.enqueue(std::to_string(i));
    }

    T_EQUAL_STR(x.dequeue().c_str(), "11");
    T_EQUAL_STR(x.dequeue().c_str(), "12");

    for (int i = 13; i <= 40; i++) {
        T_EQUAL_STR(x.dequeue().c_str(), std::to_string(i).c_str());
    }

    for (int i = 41; i <= 100; i++) {
        x.enqueue(std::to_string(i));
    }
}

void dequeue_should_busy_wait_for_data() {
    shooterstein::Queue<int> x;

    std::thread filler([&x]{
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        x.enqueue(10);
    });

    std::thread reader([&x]{
        T_CMPINT(x.dequeue(), ==, 10);
    });

    reader.join();
    filler.join();
}

void queue_should_not_have_datarace() {
    shooterstein::Queue<int> x;

    std::thread filler([&x]{
        for (int i = 0; i < 100000; i++) {
            x.enqueue(i);
        }
    });

    std::thread reader([&x]{
        for (int i = 0; i < 100000; i++) {
            T_CMPINT(x.dequeue(), ==, i);
        }
    });

    reader.join();
    filler.join();
}

int main(int argc, char* argv[]) {
    T_ADD(test_emptyqueue_should_have_length_zero);
    T_ADD(test_emptyqueue_should_be_empty);
    T_ADD(test_add_int_should_increase_size);
    T_ADD(test_add_int_should_declare_not_empty);
    T_ADD(test_add_int_should_be_dequeueable);
    T_ADD(test_dequeue_int_should_decrease_size);
    T_ADD(test_dequeue_int_should_set_empty);
    T_ADD(test_should_enqueue_copy_string);
    T_ADD(test_should_enqueue_move_string);
    T_ADD(test_should_dequeue_string);
    T_ADD(test_enqueue_dequeue_some_ints);
    T_ADD(test_enqueue_four_strings);
    T_ADD(test_enqueue_dequeue_some_strings);
    T_ADD(dequeue_should_busy_wait_for_data);
    T_ADD(queue_should_not_have_datarace);

    return T_RUN(argc, argv);
}

