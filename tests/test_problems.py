# test problems 1-8

# Problem 1: array[i]["key"] chained access
p1 = [{"title": "win1", "width": 20}]

def get_title(idx)
    return p1[idx]["title"]
end

r1 = get_title(0)
println("P1: " + r1)
