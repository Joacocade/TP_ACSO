def binary_search(sorted_list, target):
    left = 0
    right = len(sorted_list) - 1
    splits = 0

    while left <= right:
        mid = (left + right) // 2
        splits += 1

        if sorted_list[mid] == target:
            return mid, splits
        elif sorted_list[mid] < target:
            left = mid + 1
        else:
            right = mid - 1

    return -1, splits

if __name__ == "__main__":
    with open("bomb28/palabras.txt", "r") as file:
        lines = file.readlines()
        strings = sorted([line.strip() for line in lines if ('ñ' not in line and 'ü' not in line)])

    solutions = []
    for target_string in strings:
        index, split_count = binary_search(strings, target_string)

        if index != -1 and (6 < split_count <= 11):
            solutions.append([target_string, split_count])

    print("Solutions:")
    for solution in solutions[:100]:
        print(f"Found '{solution[0]}' after {solution[1]} splits.")