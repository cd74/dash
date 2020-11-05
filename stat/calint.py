import numpy as np

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
            low.append(ans[intl])
            high.append(ans[-intl])
        np.save(name+'l.npy',low)
        np.save(name+'h.npy',high)

for a in ['0.00002','0.00003','0.00004','0.0000','0.00006']:
    for b in ['t5jt','t6jt','t7jt','t8jt','t9jt','t10jt','t11jt','njt','']:
        calint(a+b)

