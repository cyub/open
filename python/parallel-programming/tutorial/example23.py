import multiprocessing


def worker(dictionary, key, item):
    dictionary[key] = item
    print(
        "process = %s, key = %d value = %d"
        % (multiprocessing.current_process().name, key, item)
    )


if __name__ == "__main__":
    mgr = multiprocessing.Manager()
    dictionary = mgr.dict()
    jobs = [
        multiprocessing.Process(target=worker, args=(dictionary, i, i * i))
        for i in range(0, 5)
    ]

    for j in jobs:
        j.start()

    for j in jobs:
        j.join()

    print("results: ", dictionary)
