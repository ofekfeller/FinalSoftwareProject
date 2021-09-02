import sys
import numpy as np
import pandas as pd
import spkmeansmodule


def read_csv_files_to_numpy(filename1):

    file = pd.read_csv(filename1, header=None)

    return file.to_numpy()


def find_min_d(vec, centers):
    min_val = np.infty
    for i in range(len(centers)):
        norm = np.linalg.norm(np.subtract(centers[i], vec))
        norm = norm**2
        if norm < min_val:
            min_val = norm

    return min_val


# is there better approach we can handle this with?
def is_int(num):
    int(num)


def get_args(args):
    if len(args) == 4:
        return int(args[1]), args[2], args[3]
    else:
        raise Exception("Arguments are corrupted")


def main():
    np.random.seed(0)
    _K, goal, filename = get_args(sys.argv)
    points = read_csv_files_to_numpy(filename)

    if goal != "spk":
        spkmeansmodule.execute_goal(points.tolist(), goal)
        return

    assert (_K < points.shape[0])

    spk_points = spkmeansmodule.spk_points(points.tolist(), [1 for i in range(_K)])

    spk_points = np.asarray(spk_points)

    _K = spk_points.shape[1]  # the new K is the number of columns

    z = 1
    points_num = spk_points.shape[0]

    rnd = np.random.choice(range(points_num))
    indexes = str(rnd)
    centers = [spk_points[rnd, :]]
    for i in range(_K-1):
        prob_vec = [find_min_d(spk_points[j, :], centers) for j in range(points_num)]
        total = sum(prob_vec)
        prob_vec = [val/total for val in prob_vec]
        p = np.random.choice([i for i in range(points_num)], p=prob_vec)
        indexes += f",{p}"
        centers.append(spk_points[p, :])

    print(indexes)

    centers = [center.tolist() for center in centers]

    spkmeansmodule.fit(spk_points.tolist(), centers)

    #res = np.round(np.array(res), decimals=4)

    #for row in res:
    #    print(",".join(map(str, row)))


if __name__ == '__main__':
    main()
