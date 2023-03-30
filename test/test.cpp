#include <iostream>
#include <thread>
#include <future>

// Function to be executed on the thread
void computeResult(int a, int b, std::promise<int> &resultPromise)
{
    // Do some computation using the parameters
    int result = a + b;

    // Set the result on the promise object
    resultPromise.set_value(result);
}

int main()
{
    // Create a promise object
    std::promise<int> resultPromise;

    // Get a future object from the promise
    std::future<int> resultFuture = resultPromise.get_future();

    // Create a thread and pass the parameters and the promise object to the function
    std::thread myThread(computeResult, 2, 3, std::ref(resultPromise));

    // Loop 5 times to call the function with different parameters on the existing thread
    for (int i = 0; i < 5; i++)
    {
        // Call the function on the existing thread with different parameters
        myThread = std::thread(computeResult, i, i + 1, std::ref(resultPromise));

        // Wait for the thread to finish executing
        myThread.join();

        // Get the result from the future object
        int result = resultFuture.get();

        // Print the result
        std::cout << "Result: " << result << std::endl;
    }

    return 0;
}
