#ifndef __MONOTONE_SPLINE_H_INCLUDED__
#define __MONOTONE_SPLINE_H_INCLUDED__
#include <vector>
#include <cmath>
namespace spline{

    //xs needs to be sorted
    template<typename Arr>
    auto generateSpline(Arr&& xs, Arr&& ys){
        int n=xs.size();
        Arr dxs; //length n-1
        Arr ms; //length n-1
        
        for (int i = 0; i < n-1; ++i) {
            auto dx=xs[i+1]-xs[i];
            auto dy=ys[i+1]-ys[i];
            dxs.emplace_back(dx);
            ms.emplace_back(dy/dx);
        }
        Arr c1s; //length n
        c1s.emplace_back(ms[0]);
        for(int i=0; i<n-2;++i){
            auto m=ms[i];
            auto mNext=ms[i+1];
            if(m*mNext<=0){
                c1s.emplace_back(0);
            }
            else{
                auto dx=dxs[i];
                auto dxNext=dxs[i+1];
                auto common=dx+dxNext;
                c1s.emplace_back(3*common/((common+dxNext)/m+(common+dx)/mNext));
            }
        }
        c1s.emplace_back(ms.back());

        Arr c2s; //length n-1
        Arr c3s; //length n-1
        for(int i=0; i< n-1;++i){
            auto c1=c1s[i];
            auto m=ms[i];
            auto invDx=1.0/dxs[i];
            auto common=c1+c1s[i+1]-2.0*m;
            c2s.emplace_back((m-c1-common)*invDx);
            c3s.emplace_back(common*invDx*invDx);
        }
        return [
            xs=std::move(xs),//length n
            ys=std::move(ys),//length n
            c1s=std::move(c1s), //length n
            c2s=std::move(c2s), //length n-1
            c3s=std::move(c3s), //length n-1
            n=std::move(n)
        ](const auto& x){
            if(x==xs.back()){
                return ys.back();
            }
            auto low=0;
            auto mid=0;
            auto high=n-2; ///hmm
            while(low<=high){
                mid=std::floor(.5*(low+high));
                auto xHere=xs[mid];
                if(xHere<x){
                    low=mid+1;
                }
                else if(xHere>x){
                    high=mid-1;
                }
                else{
                    return ys[mid];
                }
            }
            int index=high>0?high:0;
            auto diff=x-xs[index];
            auto diffsq=diff*diff;
            return ys[index]+c1s[index]*diff+c2s[index]*diffsq+c3s[index]*diff*diffsq;
         
        };
    }

}


#endif