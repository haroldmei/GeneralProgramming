import sys

def calculateTax1(income):
    # Write your code here
    tax0 = 0.0      # add for 18201~37000
    tax1 = 3572.0   # add for 37001~87000
    tax2 = 16250.0  # add for 87001~180000
    tax3 = 34410.0  # add for 180001 ~
    total = 0.0
    if income < 18200:
        total = 0.0
    elif income < 37000:
        total = tax0 + (income - 18200) * 0.19
    elif income < 87000:
        total = tax0 + tax1 + (income - 37000) * 0.325
    elif income < 180000:
        total = tax0 + tax1 + tax2 + (income - 87000) * 0.37
    else:
        total = tax0 + tax1 + tax2 + tax3 + (income - 180000) * 0.45

    return "{:.2f}".format(total)

def calculateTax(income):
    # Write your code here
    taxs = [0.0, 3572.0, 19822.0, 54232.0]
    incomes = [18200, 37000, 87000, 180000, sys.maxsize]
    total = 0.0
    if income < 18200:
        total = 0.0
    elif income < 37000:
        total = tax0 + (income - 18200) * 0.19
    elif income < 87000:
        total = tax0 + tax1 + (income - 37000) * 0.325
    elif income < 180000:
        total = tax0 + tax1 + tax2 + (income - 87000) * 0.37
    else:
        total = tax0 + tax1 + tax2 + tax3 + (income - 180000) * 0.45

    return "{:.2f}".format(total)

print(calculateTax(37009))
print(calculateTax(37001))
print(calculateTax(1))