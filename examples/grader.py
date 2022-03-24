from glob import glob 
import os 

EXAMPLE_DIR = 'examples'
PCL_DIR = '../src/'
SAVE_DIR = 'grader_info'

print("#"*20)
print('Testing')
print("#"*20)

os.chdir(PCL_DIR)
for example_set in ['correct','wrong'] :

    path_test = f'../{EXAMPLE_DIR}/' + example_set + '/'
    test_paths = glob(path_test + '*.pcl')
    test_paths.sort()
    
    print(path_test)
    log_path = f'../{EXAMPLE_DIR}/' + SAVE_DIR + '/' + example_set + '/'
    os.makedirs(f'{log_path}logs/', exist_ok=True)
    os.makedirs(f'{log_path}runs/', exist_ok=True)
    print(log_path)
    for test_path in test_paths : 
        test_base = os.path.splitext(test_path)[0]
        filename = test_base.split('/')[-1]
        dir_path = '/'.join(test_base.split('/')[:-1])

        # compile and redirect output to log file

        cmd_comp = f' ./pcl {test_path} 2>&1 | tee {log_path}logs/{filename}.log '
        cmd_run = f'script -q -c {dir_path}/{filename}.out  /dev/null 2>&1 | tee  {log_path}runs/{filename}.run.log'
        
        print(f'{example_set} -- {filename}')
        os.system(cmd_comp)
        os.system(cmd_run)
        break 
