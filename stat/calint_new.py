import numpy as np

from scipy import stats

def calint(name):
    try:
        x=list(np.load(name+'.npy'))
    except:
        return 
    else:
        low,high = [],[]
        for i in range(1,121):
            ans=[]
            for item in x:
                if(item.get(i)):
                    ans.append(item.get(i))
            intl=round(len(ans)/20)
            ans.sort()
            array=np.array(ans)
            mean,std=array.mean(),np.std(array)            
            l1,h1=stats.norm.interval(0.95,mean,std/np.sqrt(len(ans))) 
            low.append(l1)
            high.append(h1)
        np.save(name+'l.npy',low)
        np.save(name+'h.npy',high)

for a in ['0.00002','0.00003','0.00004','0.0000','0.00006']:
    for b in ['t5jt','t6jt','t7jt','t8jt','t9jt','t10jt','t11jt','njt','','t5','t6','t7','t8','t9','t10','t11','n']:
        calint(a+b)

