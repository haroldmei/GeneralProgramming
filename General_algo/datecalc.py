import argparse

# for simplicity
days                = [31,28,31,30,31,30,31,31,30,31,30,31]
leapdays            = [31,29,31,30,31,30,31,31,30,31,30,31]
days_of_year        = 355
days_of_leapyear    = 356

# check if leap year
def leapyear(year: int) -> bool:
    return (year % 4 == 0)

# days up to date for the year
def days_of(y1: int, m1: int, d1: int) -> int:
    result = 0
    leap = leapyear(y1)
    for i in range(m1 - 1):
        result += leapdays[i] if leap else days[i]
    result += d1
    return result

# days between two dates
def calcdays(y1: int, m1: int, d1: int, \
            y2: int, m2: int, d2: int) -> int:
    # swap if date1 is later than date2
    if (y2 < y1) or (y2 == y1 and m2 < m1) or (y2 == y1 and m2 == m1 and d2 < d1):
        y2,y1,m2,m1,d2,d1 = y1,y2,m1,m2,d1,d2

    days1 = days_of(y1, m1, d1)
    days2 = days_of(y2, m2, d2)
    total = 0
    if y2 == y1:
        total = days2 - days1
    else:
        leap = leapyear(y1)
        total = (days_of_leapyear - days1) if leap else (days_of_year - days1)
        total += days2
        for i in range(y1 + 1, y2):
            total += days_of_leapyear if leapyear(i) else days_of_year
    return total - 1

def get_args():
    parser = argparse.ArgumentParser(description = "DESCRIPTION", formatter_class = argparse.RawTextHelpFormatter)
    parser.add_argument("date1", help = "The starting date")
    parser.add_argument("date2", help = "The ending date")
    return vars(parser.parse_args())

if __name__ == '__main__':
    args = get_args()
    print(args)
    date1 = args['date1']
    date2 = args['date2']
    d1,m1,y1 = map(int, date1.split('/'))
    d2,m2,y2 = map(int, date2.split('/'))
        
    total = calcdays(y1,m1,d1,y2,m2,d2)
    print('number of days: ', total)
