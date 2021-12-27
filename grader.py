from glob import glob 
import os 
BASE = './pcl_examples/'
print("#"*20)
print('Testing')
print("#"*20)
for ex_set in ['correct','wrong','official','neg','pos'] :
    path_save = BASE + ex_set + '/'
    exs = glob(path_save + '*.pcl')
    exs.sort()
    os.makedirs(f'{path_save}logs/', exist_ok=True)
    os.makedirs(f'{path_save}runs/', exist_ok=True)
    for ex_path in exs : 
        #import pdb; pdb.set_trace()
        ex_base = os.path.splitext(ex_path)[0]
        filename = ex_base.split('/')[-1]
        # compile and redirect output to log file
        cmd_comp = f'./pcl {ex_path} 2>&1 | tee  {path_save}logs/{filename}.log '
        cmd_run = f'script -q -c ./{filename}.out  2>&1 | tee  {path_save}runs/{filename}.run.log'
        print(f'{ex_set} -- {filename}')
        os.system(cmd_comp)
        os.system(cmd_run)
