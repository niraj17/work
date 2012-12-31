
def get_min_coin_configuration(sum = None, coins = None, cache = None):
if cache == None: # this is quite crucial if its in the definition its presistent ...
    cache = {}
if sum in cache:
    return cache[sum]
elif sum in coins: # if sum in coins, nothing to do but return.
    cache[sum] = [sum]
    return cache[sum]
elif max(coins) > sum: # if the largest coin is greater then the sum, there's nothing we can do.
    cache[sum] = None
    return cache[sum]
else: # check for each coin, keep track of the minimun configuration, then return it.
    min_length = None
    min_configuration = None
    for coin in coins:
        results = get_min_coin_configuration(sum = sum - coin, coins = coins, cache = cache)
        if results != None:
            if min_length == None or (1 + len(results)) < len(min_configuration):
                min_configuration = [coin] + results
                min_length = len(min_configuration)
    cache[sum] = min_configuration
    return cache[sum]
