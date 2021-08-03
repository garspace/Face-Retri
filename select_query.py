import os, random, shutil

random.seed(12)
num_query = 100
imgs_path = "/home/muyu/Downloads/face_base/"
query_path = "/home/muyu/Downloads/query_path/"

if not os.path.exists(query_path):
    os.mkdir(query_path)

num_images = len(os.listdir(imgs_path))
select = random.sample(range(0, num_images), num_query)
select.sort()

print(select)

for idx, file in enumerate(os.listdir(imgs_path)):
    if len(select) > 0 and idx == select[0]:
        shutil.copy(imgs_path+file, query_path+file)
        select.pop(0)
    if (len(select) == 0):
        break

# [27, 177, 304, 499, 841, 963, 995, 1222, 1354, 1369, 1431, 
# 1490, 1843, 1865, 2212, 2336, 2380, 2658, 2679, 2805, 3143, 
# 3270, 3444, 3657, 3729, 3849, 4134, 4285, 4407, 4490, 5077, 
# 5120, 5492, 5521, 5565, 5730, 5838, 5949, 5952, 6023, 6082, 
# 6139, 6252, 6300, 6317, 6562, 6629, 6862, 6933, 6943, 7209, 
# 7270, 7540, 7693, 7775, 7811, 7905, 8310, 8338, 8377, 8413, 
# 8669, 8839, 8866, 9095, 9144, 9145, 9391, 9440, 9764, 9854, 
# 9952, 9982, 10181, 10227, 10497, 10541, 10772, 10827, 10835, 
# 10846, 10872, 10918, 10946, 11039, 11164, 11254, 11306, 11314, 
# 11416, 11900, 12535, 12899, 13072, 13272, 13276, 13364, 13415, 
# 13422, 13429]
