/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggSQUISH SOFTWARE CODEC SOURCE CODE.   *
 *                                                                  *
 ********************************************************************



> From xiphmont@MIT.EDU Tue Jun  3 14:50:15 1997
> Subject: Re: fft.c 
> Date: Tue, 03 Jun 1997 14:50:11 EDT
> From: Monty <xiphmont@MIT.EDU>
> 
> The include files are not really necessary; they define a few macros
> depending on the compiler in use (eg, STIN becomes one of "static" or
> "static inline") and the prototypes for the functions in fft.c.  You
> should be able to compile and use the code without the headers with
> minimal modification.  The C code behaves nearly identically to the
> fortran code; inspection of the code should clarify the differences
> (eg, the 'ifac' argument).  Let me know if you need any other help
> with the code.
> 
> Monty




  file: fft.c
  function: Fast discrete Fourier and cosine transforms and inverses
  author: Monty <xiphmont@mit.edu>
  modifications by: Monty
  last modification date: Jul 1 1996

 ********************************************************************/

/* These Fourier routines were originally based on the Fourier
   routines of the same names from the NETLIB bihar and fftpack
   fortran libraries developed by Paul N. Swarztrauber at the National
   Center for Atmospheric Research in Boulder, CO USA.  They have been
   reimplemented in C and optimized in a few ways for OggSquish. */

/* As the original fortran libraries are public domain, the C Fourier
   routines in this file are hereby released to the public domain as
   well.  The C routines here produce output exactly equivalent to the
   original fortran routines.  Of particular interest are the facts
   that (like the original fortran), these routines can work on
   arbitrary length vectors that need not be powers of two in
   length. */


/* file modifyed by Markus Hoffmann 2011 for the use together with 
   X11-Basic  */

#include <math.h>
//#include "ogg.h"
#include "fft.h"

#define STIN static 

static void drfti1(int n, FLOAT *wa, int *ifac){
  static const int ntryh[4] = { 4,2,3,5 };
  FLOAT arg,argh,argld,fi;
  int ntry=0,i,j=-1;
  int k1, l1, l2, ib;
  int ld, ii, ip, is, nq, nr;
  int ido, ipm, nfm1;
  int nl=n;
  int nf=0;

 L101:
  j++;
  if (j<4) ntry=ntryh[j];
  else     ntry+=2;

 L104:
  nq=nl/ntry;
  nr=nl-ntry*nq;
  if (nr!=0) goto L101;

  nf++;
  ifac[nf+1]=ntry;
  nl=nq;
  if(ntry!=2)goto L107;
  if(nf==1)goto L107;

  for (i=1;i<nf;i++){
    ib=nf-i+1;
    ifac[ib+1]=ifac[ib];
  }
  ifac[2] = 2;

 L107:
  if(nl!=1)goto L104;
  ifac[0]=n;
  ifac[1]=nf;
  argh=TWOPI/n;
  is=0;
  nfm1=nf-1;
  l1=1;

  if(nfm1==0)return;

  for (k1=0;k1<nfm1;k1++){
    ip=ifac[k1+2];
    ld=0;
    l2=l1*ip;
    ido=n/l2;
    ipm=ip-1;

    for (j=0;j<ipm;j++){
      ld+=l1;
      i=is;
      argld=(FLOAT)ld*argh;
      fi=0.;
      for (ii=2;ii<ido;ii+=2){
	fi+=1.;
	arg=fi*argld;
	wa[i++]=cos(arg);
	wa[i++]=sin(arg);
      }
      is+=ido;
    }
    l1=l2;
  }
}

void rffti(int n, FLOAT *wsave, int *ifac){
  if (n == 1) return;
  drfti1(n, wsave+n, ifac);
}
#if 0
void __ogg_fdcosqi(int n, FLOAT *wsave, int *ifac){
  static int k;
  static FLOAT fk, dt;

  dt=PIHALF/n;
  fk=0.;
  for(k=0;k<n;k++){
    fk+=1.;
    wsave[k] = cos(fk*dt);
  }

  rffti(n, wsave+n,ifac);
}
#endif
STIN void dradf2(int ido,int l1,FLOAT *cc,FLOAT *ch,FLOAT *wa1){
  int i,k;
  FLOAT ti2,tr2;
  int t0,t1,t2,t3,t4,t5,t6;
  
  t1=0;
  t0=(t2=l1*ido);
  t3=ido<<1;
  for(k=0;k<l1;k++){
    ch[t1<<1]=cc[t1]+cc[t2];
    ch[(t1<<1)+t3-1]=cc[t1]-cc[t2];
    t1+=ido;
    t2+=ido;
  }

  if(ido<2)return;
  if(ido==2)goto L105;

  t1=0;
  t2=t0;
  for(k=0;k<l1;k++){
    t3=t2;
    t4=(t1<<1)+(ido<<1);
    t5=t1;
    t6=t1+t1;
    for(i=2;i<ido;i+=2){
      t3+=2;
      t4-=2;
      t5+=2;
      t6+=2;
      tr2=wa1[i-2]*cc[t3-1]+wa1[i-1]*cc[t3];
      ti2=wa1[i-2]*cc[t3]-wa1[i-1]*cc[t3-1];
      ch[t6]=cc[t5]+ti2;
      ch[t4]=ti2-cc[t5];
      ch[t6-1]=cc[t5-1]+tr2;
      ch[t4-1]=cc[t5-1]-tr2;
    }
    t1+=ido;
    t2+=ido;
  }

  if(ido%2==1)return;

 L105:
  t3=(t2=(t1=ido)-1);
  t2+=t0;
  for(k=0;k<l1;k++){
    ch[t1]=-cc[t2];
    ch[t1-1]=cc[t3];
    t1+=ido<<1;
    t2+=ido;
    t3+=ido;
  }
}

STIN void dradf4(int ido,int l1,FLOAT *cc,FLOAT *ch,FLOAT *wa1,
	    FLOAT *wa2,FLOAT *wa3){
  static const FLOAT hsqt2 = .70710678118654752440084436210485;
  int i,k,t0,t1,t2,t3,t4,t5,t6;
  FLOAT ci2,ci3,ci4,cr2,cr3,cr4,ti1,ti2,ti3,ti4,tr1,tr2,tr3,tr4;
  t0=l1*ido;
  
  t1=t0;
  t4=t1<<1;
  t2=t1+(t1<<1);
  t3=0;

  for(k=0;k<l1;k++){
    tr1=cc[t1]+cc[t2];
    tr2=cc[t3]+cc[t4];
    ch[t5=t3<<2]=tr1+tr2;
    ch[(ido<<2)+t5-1]=tr2-tr1;
    ch[(t5+=(ido<<1))-1]=cc[t3]-cc[t4];
    ch[t5]=cc[t2]-cc[t1];

    t1+=ido;
    t2+=ido;
    t3+=ido;
    t4+=ido;
  }

  if(ido<2)return;
  if(ido==2)goto L105;

  t1=0;
  for(k=0;k<l1;k++){
    t2=t1;
    t4=t1<<2;
    t5=(t6=ido<<1)+t4;
    for(i=2;i<ido;i+=2){
      t3=(t2+=2);
      t4+=2;
      t5-=2;

      t3+=t0;
      cr2=wa1[i-2]*cc[t3-1]+wa1[i-1]*cc[t3];
      ci2=wa1[i-2]*cc[t3]-wa1[i-1]*cc[t3-1];
      t3+=t0;
      cr3=wa2[i-2]*cc[t3-1]+wa2[i-1]*cc[t3];
      ci3=wa2[i-2]*cc[t3]-wa2[i-1]*cc[t3-1];
      t3+=t0;
      cr4=wa3[i-2]*cc[t3-1]+wa3[i-1]*cc[t3];
      ci4=wa3[i-2]*cc[t3]-wa3[i-1]*cc[t3-1];

      tr1=cr2+cr4;
      tr4=cr4-cr2;
      ti1=ci2+ci4;
      ti4=ci2-ci4;
      ti2=cc[t2]+ci3;
      ti3=cc[t2]-ci3;
      tr2=cc[t2-1]+cr3;
      tr3=cc[t2-1]-cr3;

      
      ch[t4-1]=tr1+tr2;
      ch[t4]=ti1+ti2;

      ch[t5-1]=tr3-ti4;
      ch[t5]=tr4-ti3;

      ch[t4+t6-1]=ti4+tr3;
      ch[t4+t6]=tr4+ti3;

      ch[t5+t6-1]=tr2-tr1;
      ch[t5+t6]=ti1-ti2;
    }
    t1+=ido;
  }
  if(ido%2==1)return;

 L105:
  
  t2=(t1=t0+ido-1)+(t0<<1);
  t3=ido<<2;
  t4=ido;
  t5=ido<<1;
  t6=ido;

  for(k=0;k<l1;k++){
    ti1=-hsqt2*(cc[t1]+cc[t2]);
    tr1=hsqt2*(cc[t1]-cc[t2]);
    ch[t4-1]=tr1+cc[t6-1];
    ch[t4+t5-1]=cc[t6-1]-tr1;
    ch[t4]=ti1-cc[t1+t0];
    ch[t4+t5]=ti1+cc[t1+t0];
    t1+=ido;
    t2+=ido;
    t4+=t3;
    t6+=ido;
  }
}

STIN void dradfg(int ido,int ip,int l1,int idl1,FLOAT *cc,FLOAT *c1,
			  FLOAT *c2,FLOAT *ch,FLOAT *ch2,FLOAT *wa){
  int idij,ipph,i,j,k,l,ic,ik,is;
  int t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
  FLOAT dc2,ai1,ai2,ar1,ar2,ds2;
  int nbd;
  FLOAT dcp,arg,dsp,ar1h,ar2h;
  int idp2,ipp2;
  
  arg=TWOPI/(FLOAT)ip;
  dcp=cos(arg);
  dsp=sin(arg);
  ipph=(ip+1)>>1;
  ipp2=ip;
  idp2=ido;
  nbd=(ido-1)>>1;
  t0=l1*ido;
  t10=ip*ido;

  if(ido==1)goto L119;
  for(ik=0;ik<idl1;ik++)ch2[ik]=c2[ik];

  t1=0;
  for(j=1;j<ip;j++){
    t1+=t0;
    t2=t1;
    for(k=0;k<l1;k++){
      ch[t2]=c1[t2];
      t2+=ido;
    }
  }

  is=-ido;
  t1=0;
  if(nbd>l1){
    for(j=1;j<ip;j++){
      t1+=t0;
      is+=ido;
      t2= -ido+t1;
      for(k=0;k<l1;k++){
	idij=is-1;
	t2+=ido;
	t3=t2;
	for(i=2;i<ido;i+=2){
	  idij+=2;
	  t3+=2;
	  ch[t3-1]=wa[idij-1]*c1[t3-1]+wa[idij]*c1[t3];
	  ch[t3]=wa[idij-1]*c1[t3]-wa[idij]*c1[t3-1];
	}
      }
    }
  }else{

    for(j=1;j<ip;j++){
      is+=ido;
      idij=is-1;
      t1+=t0;
      t2=t1;
      for(i=2;i<ido;i+=2){
	idij+=2;
	t2+=2;
	t3=t2;
	for(k=0;k<l1;k++){
	  ch[t3-1]=wa[idij-1]*c1[t3-1]+wa[idij]*c1[t3];
	  ch[t3]=wa[idij-1]*c1[t3]-wa[idij]*c1[t3-1];
	  t3+=ido;
	}
      }
    }
  }

  t1=0;
  t2=ipp2*t0;
  if(nbd<l1){
    for(j=1;j<ipph;j++){
      t1+=t0;
      t2-=t0;
      t3=t1;
      t4=t2;
      for(i=2;i<ido;i+=2){
	t3+=2;
	t4+=2;
	t5=t3-ido;
	t6=t4-ido;
	for(k=0;k<l1;k++){
	  t5+=ido;
	  t6+=ido;
	  c1[t5-1]=ch[t5-1]+ch[t6-1];
	  c1[t6-1]=ch[t5]-ch[t6];
	  c1[t5]=ch[t5]+ch[t6];
	  c1[t6]=ch[t6-1]-ch[t5-1];
	}
      }
    }
  }else{
    for(j=1;j<ipph;j++){
      t1+=t0;
      t2-=t0;
      t3=t1;
      t4=t2;
      for(k=0;k<l1;k++){
	t5=t3;
	t6=t4;
	for(i=2;i<ido;i+=2){
	  t5+=2;
	  t6+=2;
	  c1[t5-1]=ch[t5-1]+ch[t6-1];
	  c1[t6-1]=ch[t5]-ch[t6];
	  c1[t5]=ch[t5]+ch[t6];
	  c1[t6]=ch[t6-1]-ch[t5-1];
	}
	t3+=ido;
	t4+=ido;
      }
    }
  }

L119:
  for(ik=0;ik<idl1;ik++)c2[ik]=ch2[ik];

  t1=0;
  t2=ipp2*idl1;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1-ido;
    t4=t2-ido;
    for(k=0;k<l1;k++){
      t3+=ido;
      t4+=ido;
      c1[t3]=ch[t3]+ch[t4];
      c1[t4]=ch[t4]-ch[t3];
    }
  }

  ar1=1.;
  ai1=0.;
  t1=0;
  t2=ipp2*idl1;
  t3=(ip-1)*idl1;
  for(l=1;l<ipph;l++){
    t1+=idl1;
    t2-=idl1;
    ar1h=dcp*ar1-dsp*ai1;
    ai1=dcp*ai1+dsp*ar1;
    ar1=ar1h;
    t4=t1;
    t5=t2;
    t6=t3;
    t7=idl1;

    for(ik=0;ik<idl1;ik++){
      ch2[t4++]=c2[ik]+ar1*c2[t7++];
      ch2[t5++]=ai1*c2[t6++];
    }

    dc2=ar1;
    ds2=ai1;
    ar2=ar1;
    ai2=ai1;

    t4=idl1;
    t5=(ipp2-1)*idl1;
    for(j=2;j<ipph;j++){
      t4+=idl1;
      t5-=idl1;

      ar2h=dc2*ar2-ds2*ai2;
      ai2=dc2*ai2+ds2*ar2;
      ar2=ar2h;

      t6=t1;
      t7=t2;
      t8=t4;
      t9=t5;
      for(ik=0;ik<idl1;ik++){
	ch2[t6++]+=ar2*c2[t8++];
	ch2[t7++]+=ai2*c2[t9++];
      }
    }
  }

  t1=0;
  for(j=1;j<ipph;j++){
    t1+=idl1;
    t2=t1;
    for(ik=0;ik<idl1;ik++)ch2[ik]+=c2[t2++];
  }

  if(ido<l1)goto L132;

  t1=0;
  t2=0;
  for(k=0;k<l1;k++){
    t3=t1;
    t4=t2;
    for(i=0;i<ido;i++)cc[t4++]=ch[t3++];
    t1+=ido;
    t2+=t10;
  }

  goto L135;

 L132:
  for(i=0;i<ido;i++){
    t1=i;
    t2=i;
    for(k=0;k<l1;k++){
      cc[t2]=ch[t1];
      t1+=ido;
      t2+=t10;
    }
  }

 L135:
  t1=0;
  t2=ido<<1;
  t3=0;
  t4=ipp2*t0;
  for(j=1;j<ipph;j++){

    t1+=t2;
    t3+=t0;
    t4-=t0;

    t5=t1;
    t6=t3;
    t7=t4;

    for(k=0;k<l1;k++){
      cc[t5-1]=ch[t6];
      cc[t5]=ch[t7];
      t5+=t10;
      t6+=ido;
      t7+=ido;
    }
  }

  if(ido==1)return;
  if(nbd<l1)goto L141;

  t1=-ido;
  t3=0;
  t4=0;
  t5=ipp2*t0;
  for(j=1;j<ipph;j++){
    t1+=t2;
    t3+=t2;
    t4+=t0;
    t5-=t0;
    t6=t1;
    t7=t3;
    t8=t4;
    t9=t5;
    for(k=0;k<l1;k++){
      for(i=2;i<ido;i+=2){
	ic=idp2-i;
	cc[i+t7-1]=ch[i+t8-1]+ch[i+t9-1];
	cc[ic+t6-1]=ch[i+t8-1]-ch[i+t9-1];
	cc[i+t7]=ch[i+t8]+ch[i+t9];
	cc[ic+t6]=ch[i+t9]-ch[i+t8];
      }
      t6+=t10;
      t7+=t10;
      t8+=ido;
      t9+=ido;
    }
  }
  return;

 L141:

  t1=-ido;
  t3=0;
  t4=0;
  t5=ipp2*t0;
  for(j=1;j<ipph;j++){
    t1+=t2;
    t3+=t2;
    t4+=t0;
    t5-=t0;
    for(i=2;i<ido;i+=2){
      t6=idp2+t1-i;
      t7=i+t3;
      t8=i+t4;
      t9=i+t5;
      for(k=0;k<l1;k++){
	cc[t7-1]=ch[t8-1]+ch[t9-1];
	cc[t6-1]=ch[t8-1]-ch[t9-1];
	cc[t7]=ch[t8]+ch[t9];
	cc[t6]=ch[t9]-ch[t8];
	t6+=t10;
	t7+=t10;
	t8+=ido;
	t9+=ido;
      }
    }
  }
}

STIN void drftf1(int n,FLOAT *c,FLOAT *ch,FLOAT *wa,int *ifac){
  int i,k1,l1,l2;
  int na,kh,nf;
  int ip,iw,ido,idl1,ix2,ix3;

  nf=ifac[1];
  na=1;
  l2=n;
  iw=n;

  for(k1=0;k1<nf;k1++){
    kh=nf-k1;
    ip=ifac[kh+1];
    l1=l2/ip;
    ido=n/l2;
    idl1=ido*l1;
    iw-=(ip-1)*ido;
    na=1-na;

    if(ip!=4)goto L102;

    ix2=iw+ido;
    ix3=ix2+ido;
    if(na!=0)
      dradf4(ido,l1,ch,c,wa+iw-1,wa+ix2-1,wa+ix3-1);
    else
      dradf4(ido,l1,c,ch,wa+iw-1,wa+ix2-1,wa+ix3-1);
    goto L110;

 L102:
    if(ip!=2)goto L104;
    if(na!=0)goto L103;

    dradf2(ido,l1,c,ch,wa+iw-1);
    goto L110;

  L103:
    dradf2(ido,l1,ch,c,wa+iw-1);
    goto L110;

  L104:
    if(ido==1)na=1-na;
    if(na!=0)goto L109;

    dradfg(ido,ip,l1,idl1,c,c,c,ch,ch,wa+iw-1);
    na=1;
    goto L110;

  L109:
    dradfg(ido,ip,l1,idl1,ch,ch,ch,c,c,wa+iw-1);
    na=0;

  L110:
    l2=l1;
  }

  if(na==1)return;

  for(i=0;i<n;i++)c[i]=ch[i];
}

void rfftf(int n,FLOAT *r,FLOAT *wsave,int *ifac){
  if(n==1)return;
  drftf1(n,r,wsave,wsave+n,ifac);
}
#if 0
STIN void dcsqf1(int n,FLOAT *x,FLOAT *w,FLOAT *xh,int *ifac){
  int modn,i,k,kc;
  int np2,ns2;
  FLOAT xim1;

  ns2=(n+1)>>1;
  np2=n;

  kc=np2;
  for(k=1;k<ns2;k++){
    kc--;
    xh[k]=x[k]+x[kc];
    xh[kc]=x[k]-x[kc];
  }

  modn=n%2;
  if(modn==0)xh[ns2]=x[ns2]+x[ns2];

  for(k=1;k<ns2;k++){
    kc=np2-k;
    x[k]=w[k-1]*xh[kc]+w[kc-1]*xh[k];
    x[kc]=w[k-1]*xh[k]-w[kc-1]*xh[kc];
  }

  if(modn==0)x[ns2]=w[ns2-1]*xh[ns2];

  rfftf(n,x,xh,ifac);

  for(i=2;i<n;i+=2){
    xim1=x[i-1]-x[i];
    x[i]=x[i-1]+x[i];
    x[i-1]=xim1;
  }
}

void __ogg_fdcosqf(int n,FLOAT *x,FLOAT *wsave,int *ifac){
    FLOAT tsqx;

  switch(n){
  case 0:case 1:
    return;
  case 2:
    tsqx=SQRT2*x[1];
    x[1]=x[0]-tsqx;
    x[0]+=tsqx;
    return;
  default:
    dcsqf1(n,x,wsave,wsave+n,ifac);
    return;
  }
}
#endif
STIN void dradb2(int ido,int l1,FLOAT *cc,FLOAT *ch,FLOAT *wa1){
  int i,k,t0,t1,t2,t3,t4,t5,t6;
  FLOAT ti2,tr2;

  t0=l1*ido;
  
  t1=0;
  t2=0;
  t3=(ido<<1)-1;
  for(k=0;k<l1;k++){
    ch[t1]=cc[t2]+cc[t3+t2];
    ch[t1+t0]=cc[t2]-cc[t3+t2];
    t2=(t1+=ido)<<1;
  }

  if(ido<2)return;
  if(ido==2)goto L105;

  t1=0;
  t2=0;
  for(k=0;k<l1;k++){
    t3=t1;
    t5=(t4=t2)+(ido<<1);
    t6=t0+t1;
    for(i=2;i<ido;i+=2){
      t3+=2;
      t4+=2;
      t5-=2;
      t6+=2;
      ch[t3-1]=cc[t4-1]+cc[t5-1];
      tr2=cc[t4-1]-cc[t5-1];
      ch[t3]=cc[t4]-cc[t5];
      ti2=cc[t4]+cc[t5];
      ch[t6-1]=wa1[i-2]*tr2-wa1[i-1]*ti2;
      ch[t6]=wa1[i-2]*ti2+wa1[i-1]*tr2;
    }
    t2=(t1+=ido)<<1;
  }

  if(ido%2==1)return;

L105:
  t1=ido-1;
  t2=ido-1;
  for(k=0;k<l1;k++){
    ch[t1]=cc[t2]+cc[t2];
    ch[t1+t0]=-(cc[t2+1]+cc[t2+1]);
    t1+=ido;
    t2+=ido<<1;
  }
}

STIN void dradb3(int ido,int l1,FLOAT *cc,FLOAT *ch,FLOAT *wa1,
			  FLOAT *wa2){
  static const FLOAT taur = -.5;
  static const FLOAT taui = .86602540378443864676372317075293618;
  int i,k,t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
  FLOAT ci2,ci3,di2,di3,cr2,cr3,dr2,dr3,ti2,tr2;
  t0=l1*ido;

  t1=0;
  t2=t0<<1;
  t3=ido<<1;
  t4=ido+(ido<<1);
  t5=0;
  for(k=0;k<l1;k++){
    tr2=cc[t3-1]+cc[t3-1];
    cr2=cc[t5]+(taur*tr2);
    ch[t1]=cc[t5]+tr2;
    ci3=taui*(cc[t3]+cc[t3]);
    ch[t1+t0]=cr2-ci3;
    ch[t1+t2]=cr2+ci3;
    t1+=ido;
    t3+=t4;
    t5+=t4;
  }

  if(ido==1)return;

  t1=0;
  t3=ido<<1;
  for(k=0;k<l1;k++){
    t7=t1+(t1<<1);
    t6=(t5=t7+t3);
    t8=t1;
    t10=(t9=t1+t0)+t0;

    for(i=2;i<ido;i+=2){
      t5+=2;
      t6-=2;
      t7+=2;
      t8+=2;
      t9+=2;
      t10+=2;
      tr2=cc[t5-1]+cc[t6-1];
      cr2=cc[t7-1]+(taur*tr2);
      ch[t8-1]=cc[t7-1]+tr2;
      ti2=cc[t5]-cc[t6];
      ci2=cc[t7]+(taur*ti2);
      ch[t8]=cc[t7]+ti2;
      cr3=taui*(cc[t5-1]-cc[t6-1]);
      ci3=taui*(cc[t5]+cc[t6]);
      dr2=cr2-ci3;
      dr3=cr2+ci3;
      di2=ci2+cr3;
      di3=ci2-cr3;
      ch[t9-1]=wa1[i-2]*dr2-wa1[i-1]*di2;
      ch[t9]=wa1[i-2]*di2+wa1[i-1]*dr2;
      ch[t10-1]=wa2[i-2]*dr3-wa2[i-1]*di3;
      ch[t10]=wa2[i-2]*di3+wa2[i-1]*dr3;
    }
    t1+=ido;
  }
}

STIN void dradb4(int ido,int l1,FLOAT *cc,FLOAT *ch,FLOAT *wa1,
			  FLOAT *wa2,FLOAT *wa3){
  int i,k,t0,t1,t2,t3,t4,t5,t6,t7,t8;
  FLOAT ci2,ci3,ci4,cr2,cr3,cr4,ti1,ti2,ti3,ti4,tr1,tr2,tr3,tr4;
  t0=l1*ido;
  
  t1=0;
  t2=ido<<2;
  t3=0;
  t6=ido<<1;
  for(k=0;k<l1;k++){
    t4=t3+t6;
    t5=t1;
    tr3=cc[t4-1]+cc[t4-1];
    tr4=cc[t4]+cc[t4]; 
    tr1=cc[t3]-cc[(t4+=t6)-1];
    tr2=cc[t3]+cc[t4-1];
    ch[t5]=tr2+tr3;
    ch[t5+=t0]=tr1-tr4;
    ch[t5+=t0]=tr2-tr3;
    ch[t5+=t0]=tr1+tr4;
    t1+=ido;
    t3+=t2;
  }

  if(ido<2)return;
  if(ido==2)goto L105;

  t1=0;
  for(k=0;k<l1;k++){
    t5=(t4=(t3=(t2=t1<<2)+t6))+t6;
    t7=t1;
    for(i=2;i<ido;i+=2){
      t2+=2;
      t3+=2;
      t4-=2;
      t5-=2;
      t7+=2;
      ti1=cc[t2]+cc[t5];
      ti2=cc[t2]-cc[t5];
      ti3=cc[t3]-cc[t4];
      tr4=cc[t3]+cc[t4];
      tr1=cc[t2-1]-cc[t5-1];
      tr2=cc[t2-1]+cc[t5-1];
      ti4=cc[t3-1]-cc[t4-1];
      tr3=cc[t3-1]+cc[t4-1];
      ch[t7-1]=tr2+tr3;
      cr3=tr2-tr3;
      ch[t7]=ti2+ti3;
      ci3=ti2-ti3;
      cr2=tr1-tr4;
      cr4=tr1+tr4;
      ci2=ti1+ti4;
      ci4=ti1-ti4;

      ch[(t8=t7+t0)-1]=wa1[i-2]*cr2-wa1[i-1]*ci2;
      ch[t8]=wa1[i-2]*ci2+wa1[i-1]*cr2;
      ch[(t8+=t0)-1]=wa2[i-2]*cr3-wa2[i-1]*ci3;
      ch[t8]=wa2[i-2]*ci3+wa2[i-1]*cr3;
      ch[(t8+=t0)-1]=wa3[i-2]*cr4-wa3[i-1]*ci4;
      ch[t8]=wa3[i-2]*ci4+wa3[i-1]*cr4;
    }
    t1+=ido;
  }

  if(ido%2 == 1)return;

 L105:

  t1=ido;
  t2=ido<<2;
  t3=ido-1;
  t4=ido+(ido<<1);
  for(k=0;k<l1;k++){
    t5=t3;
    ti1=cc[t1]+cc[t4];
    ti2=cc[t4]-cc[t1];
    tr1=cc[t1-1]-cc[t4-1];
    tr2=cc[t1-1]+cc[t4-1];
    ch[t5]=tr2+tr2;
    ch[t5+=t0]=SQRT2*(tr1-ti1);
    ch[t5+=t0]=ti2+ti2;
    ch[t5+=t0]=-SQRT2*(tr1+ti1);

    t3+=ido;
    t1+=t2;
    t4+=t2;
  }
}

STIN void dradbg(int ido,int ip,int l1,int idl1,FLOAT *cc,FLOAT *c1,
	    FLOAT *c2,FLOAT *ch,FLOAT *ch2,FLOAT *wa){
  int idij,ipph,i,j,k,l,ik,is,t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,
      t11,t12;
  FLOAT dc2,ai1,ai2,ar1,ar2,ds2;
  int nbd;
  FLOAT dcp,arg,dsp,ar1h,ar2h;
  int ipp2;

  t10=ip*ido;
  t0=l1*ido;
  arg=TWOPI/(FLOAT)ip;
  dcp=cos(arg);
  dsp=sin(arg);
  nbd=(ido-1)>>1;
  ipp2=ip;
  ipph=(ip+1)>>1;
  if(ido<l1)goto L103;
  
  t1=0;
  t2=0;
  for(k=0;k<l1;k++){
    t3=t1;
    t4=t2;
    for(i=0;i<ido;i++){
      ch[t3]=cc[t4];
      t3++;
      t4++;
    }
    t1+=ido;
    t2+=t10;
  }
  goto L106;

 L103:
  t1=0;
  for(i=0;i<ido;i++){
    t2=t1;
    t3=t1;
    for(k=0;k<l1;k++){
      ch[t2]=cc[t3];
      t2+=ido;
      t3+=t10;
    }
    t1++;
  }

 L106:
  t1=0;
  t2=ipp2*t0;
  t7=(t5=ido<<1);
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;
    t6=t5;
    for(k=0;k<l1;k++){
      ch[t3]=cc[t6-1]+cc[t6-1];
      ch[t4]=cc[t6]+cc[t6];
      t3+=ido;
      t4+=ido;
      t6+=t10;
    }
    t5+=t7;
  }

  if (ido == 1)goto L116;
  if(nbd<l1)goto L112;

  t1=0;
  t2=ipp2*t0;
  t7=0;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;

    t7+=(ido<<1);
    t8=t7;
    for(k=0;k<l1;k++){
      t5=t3;
      t6=t4;
      t9=t8;
      t11=t8;
      for(i=2;i<ido;i+=2){
	t5+=2;
	t6+=2;
	t9+=2;
	t11-=2;
	ch[t5-1]=cc[t9-1]+cc[t11-1];
	ch[t6-1]=cc[t9-1]-cc[t11-1];
	ch[t5]=cc[t9]-cc[t11];
	ch[t6]=cc[t9]+cc[t11];
      }
      t3+=ido;
      t4+=ido;
      t8+=t10;
    }
  }
  goto L116;

 L112:
  t1=0;
  t2=ipp2*t0;
  t7=0;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;
    t7+=(ido<<1);
    t8=t7;
    t9=t7;
    for(i=2;i<ido;i+=2){
      t3+=2;
      t4+=2;
      t8+=2;
      t9-=2;
      t5=t3;
      t6=t4;
      t11=t8;
      t12=t9;
      for(k=0;k<l1;k++){
	ch[t5-1]=cc[t11-1]+cc[t12-1];
	ch[t6-1]=cc[t11-1]-cc[t12-1];
	ch[t5]=cc[t11]-cc[t12];
	ch[t6]=cc[t11]+cc[t12];
	t5+=ido;
	t6+=ido;
	t11+=t10;
	t12+=t10;
      }
    }
  }

L116:
  ar1=1.;
  ai1=0.;
  t1=0;
  t9=(t2=ipp2*idl1);
  t3=(ip-1)*idl1;
  for(l=1;l<ipph;l++){
    t1+=idl1;
    t2-=idl1;

    ar1h=dcp*ar1-dsp*ai1;
    ai1=dcp*ai1+dsp*ar1;
    ar1=ar1h;
    t4=t1;
    t5=t2;
    t6=0;
    t7=idl1;
    t8=t3;
    for(ik=0;ik<idl1;ik++){
      c2[t4++]=ch2[t6++]+ar1*ch2[t7++];
      c2[t5++]=ai1*ch2[t8++];
    }
    dc2=ar1;
    ds2=ai1;
    ar2=ar1;
    ai2=ai1;

    t6=idl1;
    t7=t9-idl1;
    for(j=2;j<ipph;j++){
      t6+=idl1;
      t7-=idl1;
      ar2h=dc2*ar2-ds2*ai2;
      ai2=dc2*ai2+ds2*ar2;
      ar2=ar2h;
      t4=t1;
      t5=t2;
      t11=t6;
      t12=t7;
      for(ik=0;ik<idl1;ik++){
	c2[t4++]+=ar2*ch2[t11++];
	c2[t5++]+=ai2*ch2[t12++];
      }
    }
  }

  t1=0;
  for(j=1;j<ipph;j++){
    t1+=idl1;
    t2=t1;
    for(ik=0;ik<idl1;ik++)ch2[ik]+=ch2[t2++];
  }

  t1=0;
  t2=ipp2*t0;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;
    for(k=0;k<l1;k++){
      ch[t3]=c1[t3]-c1[t4];
      ch[t4]=c1[t3]+c1[t4];
      t3+=ido;
      t4+=ido;
    }
  }

  if(ido==1)goto L132;
  if(nbd<l1)goto L128;

  t1=0;
  t2=ipp2*t0;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;
    for(k=0;k<l1;k++){
      t5=t3;
      t6=t4;
      for(i=2;i<ido;i+=2){
	t5+=2;
	t6+=2;
	ch[t5-1]=c1[t5-1]-c1[t6];
	ch[t6-1]=c1[t5-1]+c1[t6];
	ch[t5]=c1[t5]+c1[t6-1];
	ch[t6]=c1[t5]-c1[t6-1];
      }
      t3+=ido;
      t4+=ido;
    }
  }
  goto L132;

 L128:
  t1=0;
  t2=ipp2*t0;
  for(j=1;j<ipph;j++){
    t1+=t0;
    t2-=t0;
    t3=t1;
    t4=t2;
    for(i=2;i<ido;i+=2){
      t3+=2;
      t4+=2;
      t5=t3;
      t6=t4;
      for(k=0;k<l1;k++){
	ch[t5-1]=c1[t5-1]-c1[t6];
	ch[t6-1]=c1[t5-1]+c1[t6];
	ch[t5]=c1[t5]+c1[t6-1];
	ch[t6]=c1[t5]-c1[t6-1];
	t5+=ido;
	t6+=ido;
      }
    }
  }

L132:
  if(ido==1)return;

  for(ik=0;ik<idl1;ik++)c2[ik]=ch2[ik];

  t1=0;
  for(j=1;j<ip;j++){
    t2=(t1+=t0);
    for(k=0;k<l1;k++){
      c1[t2]=ch[t2];
      t2+=ido;
    }
  }

  if(nbd>l1)goto L139;

  is= -ido-1;
  t1=0;
  for(j=1;j<ip;j++){
    is+=ido;
    t1+=t0;
    idij=is;
    t2=t1;
    for(i=2;i<ido;i+=2){
      t2+=2;
      idij+=2;
      t3=t2;
      for(k=0;k<l1;k++){
	c1[t3-1]=wa[idij-1]*ch[t3-1]-wa[idij]*ch[t3];
	c1[t3]=wa[idij-1]*ch[t3]+wa[idij]*ch[t3-1];
	t3+=ido;
      }
    }
  }
  return;

 L139:
  is= -ido-1;
  t1=0;
  for(j=1;j<ip;j++){
    is+=ido;
    t1+=t0;
    t2=t1;
    for(k=0;k<l1;k++){
      idij=is;
      t3=t2;
      for(i=2;i<ido;i+=2){
	idij+=2;
	t3+=2;
	c1[t3-1]=wa[idij-1]*ch[t3-1]-wa[idij]*ch[t3];
	c1[t3]=wa[idij-1]*ch[t3]+wa[idij]*ch[t3-1];
      }
      t2+=ido;
    }
  }
}

STIN void drftb1(int n, FLOAT *c, FLOAT *ch, FLOAT *wa, int *ifac){
  int i,k1,l1,l2;
  int na;
  int nf,ip,iw,ix2,ix3,ido,idl1;

  nf=ifac[1];
  na=0;
  l1=1;
  iw=1;

  for(k1=0;k1<nf;k1++){
    ip=ifac[k1 + 2];
    l2=ip*l1;
    ido=n/l2;
    idl1=ido*l1;
    if(ip!=4)goto L103;
    ix2=iw+ido;
    ix3=ix2+ido;

    if(na!=0)
      dradb4(ido,l1,ch,c,wa+iw-1,wa+ix2-1,wa+ix3-1);
    else
      dradb4(ido,l1,c,ch,wa+iw-1,wa+ix2-1,wa+ix3-1);
    na=1-na;
    goto L115;

  L103:
    if(ip!=2)goto L106;

    if(na!=0)
      dradb2(ido,l1,ch,c,wa+iw-1);
    else
      dradb2(ido,l1,c,ch,wa+iw-1);
    na=1-na;
    goto L115;

  L106:
    if(ip!=3)goto L109;

    ix2=iw+ido;
    if(na!=0)
      dradb3(ido,l1,ch,c,wa+iw-1,wa+ix2-1);
    else
      dradb3(ido,l1,c,ch,wa+iw-1,wa+ix2-1);
    na=1-na;
    goto L115;

  L109:
/*    The radix five case can be translated later..... */
/*    if(ip!=5)goto L112;

    ix2=iw+ido;
    ix3=ix2+ido;
    ix4=ix3+ido;
    if(na!=0)
      dradb5(ido,l1,ch,c,wa+iw-1,wa+ix2-1,wa+ix3-1,wa+ix4-1);
    else
      dradb5(ido,l1,c,ch,wa+iw-1,wa+ix2-1,wa+ix3-1,wa+ix4-1);
    na=1-na;
    goto L115;

  L112:*/
    if(na!=0)
      dradbg(ido,ip,l1,idl1,ch,ch,ch,c,c,wa+iw-1);
    else
      dradbg(ido,ip,l1,idl1,c,c,c,ch,ch,wa+iw-1);
    if(ido==1)na=1-na;

  L115:
    l1=l2;
    iw+=(ip-1)*ido;
  }

  if(na==0)return;

  for(i=0;i<n;i++)c[i]=ch[i];
}

void rfftb(int n, FLOAT *r, FLOAT *wsave, int *ifac){
  if (n == 1)return;
  drftb1(n, r, wsave, wsave+n, ifac);
}
#if 0
STIN void dcsqb1(int n,FLOAT *x,FLOAT *w,FLOAT *xh,int *ifac){
  int modn,i,k,kc;
  int np2,ns2;
  FLOAT xim1;

  ns2=(n+1)>>1;
  np2=n;

  for(i=2;i<n;i+=2){
    xim1=x[i-1]+x[i];
    x[i]-=x[i-1];
    x[i-1]=xim1;
  }

  x[0]+=x[0];
  modn=n%2;
  if(modn==0)x[n-1]+=x[n-1];

  rfftb(n,x,xh,ifac);

  kc=np2;
  for(k=1;k<ns2;k++){
    kc--;
    xh[k]=w[k-1]*x[kc]+w[kc-1]*x[k];
    xh[kc]=w[k-1]*x[k]-w[kc-1]*x[kc];
  }

  if(modn==0)x[ns2]=w[ns2-1]*(x[ns2]+x[ns2]);

  kc=np2;
  for(k=1;k<ns2;k++){
    kc--;
    x[k]=xh[k]+xh[kc];
    x[kc]=xh[k]-xh[kc];
  }
  x[0]+=x[0];
}

void __ogg_fdcosqb(int n,FLOAT *x,FLOAT *wsave,int *ifac){
  FLOAT x1;

  if(n<2){
    x[0]*=4;
    return;
  }
  if(n==2){
    x1=(x[0]+x[1])*4;
    x[1]=TSQRT2*(x[0]-x[1]);
    x[0]=x1;
    return;
  }
  
  dcsqb1(n,x,wsave,wsave+n,ifac);
}
#endif
/*

                      FFTPACK

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

                  version 4  april 1985

     a package of fortran subprograms for the fast fourier
      transform of periodic and other symmetric sequences

                         by

                  paul n swarztrauber

  national center for atmospheric research  boulder,colorado 80307

   which is sponsored by the national science foundation

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


this package consists of programs which perform fast fourier
transforms for both complex and real periodic sequences and
certain other symmetric sequences that are listed below.

1.   rffti     initialize  rfftf and rfftb
2.   rfftf     forward transform of a real periodic sequence
3.   rfftb     backward transform of a real coefficient array

4.   ezffti    initialize ezfftf and ezfftb
5.   ezfftf    a simplified real periodic forward transform
6.   ezfftb    a simplified real periodic backward transform

7.   sinti     initialize sint
8.   sint      sine transform of a real odd sequence

9.   costi     initialize cost
10.  cost      cosine transform of a real even sequence

11.  sinqi     initialize sinqf and sinqb
12.  sinqf     forward sine transform with odd wave numbers
13.  sinqb     unnormalized inverse of sinqf

14.  cosqi     initialize cosqf and cosqb
15.  cosqf     forward cosine transform with odd wave numbers
16.  cosqb     unnormalized inverse of cosqf

17.  cffti     initialize cfftf and cfftb
18.  cfftf     forward transform of a complex periodic sequence
19.  cfftb     unnormalized inverse of cfftf


******************************************************************

subroutine rffti(n,wsave)

  ****************************************************************

subroutine rffti initializes the array wsave which is used in
both rfftf and rfftb. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed.

output parameter

wsave   a work array which must be dimensioned at least 2*n+15.
        the same work array can be used for both rfftf and rfftb
        as long as n remains unchanged. different wsave arrays
        are required for different values of n. the contents of
        wsave must not be changed between calls of rfftf or rfftb.

******************************************************************

subroutine rfftf(n,r,wsave)

******************************************************************

subroutine rfftf computes the fourier coefficients of a real
perodic sequence (fourier analysis). the transform is defined
below at output parameter r.

input parameters

n       the length of the array r to be transformed.  the method
        is most efficient when n is a product of small primes.
        n may change so long as different work arrays are provided

r       a real array of length n which contains the sequence
        to be transformed

wsave   a work array which must be dimensioned at least 2*n+15.
        in the program that calls rfftf. the wsave array must be
        initialized by calling subroutine rffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by rfftf and rfftb.


output parameters

r       r(1) = the sum from i=1 to i=n of r(i)

        if n is even set l =n/2   , if n is odd set l = (n+1)/2

          then for k = 2,...,l

             r(2*k-2) = the sum from i = 1 to i = n of

                  r(i)*cos((k-1)*(i-1)*2*pi/n)

             r(2*k-1) = the sum from i = 1 to i = n of

                 -r(i)*sin((k-1)*(i-1)*2*pi/n)

        if n is even

             r(n) = the sum from i = 1 to i = n of

                  (-1)**(i-1)*r(i)

 *****  note
             this transform is unnormalized since a call of rfftf
             followed by a call of rfftb will multiply the input
             sequence by n.

wsave   contains results which must not be destroyed between
        calls of rfftf or rfftb.


******************************************************************

subroutine rfftb(n,r,wsave)

******************************************************************

subroutine rfftb computes the real perodic sequence from its
fourier coefficients (fourier synthesis). the transform is defined
below at output parameter r.

input parameters

n       the length of the array r to be transformed.  the method
        is most efficient when n is a product of small primes.
        n may change so long as different work arrays are provided

r       a real array of length n which contains the sequence
        to be transformed

wsave   a work array which must be dimensioned at least 2*n+15.
        in the program that calls rfftb. the wsave array must be
        initialized by calling subroutine rffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by rfftf and rfftb.


output parameters

r       for n even and for i = 1,...,n

             r(i) = r(1)+(-1)**(i-1)*r(n)

                  plus the sum from k=2 to k=n/2 of

                   2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)

                  -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

        for n odd and for i = 1,...,n

             r(i) = r(1) plus the sum from k=2 to k=(n+1)/2 of

                  2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)

                 -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

 *****  note
             this transform is unnormalized since a call of rfftf
             followed by a call of rfftb will multiply the input
             sequence by n.

wsave   contains results which must not be destroyed between
        calls of rfftb or rfftf.


******************************************************************

subroutine ezffti(n,wsave)

******************************************************************

subroutine ezffti initializes the array wsave which is used in
both ezfftf and ezfftb. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed.

output parameter

wsave   a work array which must be dimensioned at least 3*n+15.
        the same work array can be used for both ezfftf and ezfftb
        as long as n remains unchanged. different wsave arrays
        are required for different values of n.


******************************************************************

subroutine ezfftf(n,r,azero,a,b,wsave)

******************************************************************

subroutine ezfftf computes the fourier coefficients of a real
perodic sequence (fourier analysis). the transform is defined
below at output parameters azero,a and b. ezfftf is a simplified
but slower version of rfftf.

input parameters

n       the length of the array r to be transformed.  the method
        is must efficient when n is the product of small primes.

r       a real array of length n which contains the sequence
        to be transformed. r is not destroyed.


wsave   a work array which must be dimensioned at least 3*n+15.
        in the program that calls ezfftf. the wsave array must be
        initialized by calling subroutine ezffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by ezfftf and ezfftb.

output parameters

azero   the sum from i=1 to i=n of r(i)/n

a,b     for n even b(n/2)=0. and a(n/2) is the sum from i=1 to
        i=n of (-1)**(i-1)*r(i)/n

        for n even define kmax=n/2-1
        for n odd  define kmax=(n-1)/2

        then for  k=1,...,kmax

             a(k) equals the sum from i=1 to i=n of

                  2./n*r(i)*cos(k*(i-1)*2*pi/n)

             b(k) equals the sum from i=1 to i=n of

                  2./n*r(i)*sin(k*(i-1)*2*pi/n)


******************************************************************

subroutine ezfftb(n,r,azero,a,b,wsave)

******************************************************************

subroutine ezfftb computes a real perodic sequence from its
fourier coefficients (fourier synthesis). the transform is
defined below at output parameter r. ezfftb is a simplified
but slower version of rfftb.

input parameters

n       the length of the output array r.  the method is most
        efficient when n is the product of small primes.

azero   the constant fourier coefficient

a,b     arrays which contain the remaining fourier coefficients
        these arrays are not destroyed.

        the length of these arrays depends on whether n is even or
        odd.

        if n is even n/2    locations are required
        if n is odd (n-1)/2 locations are required

wsave   a work array which must be dimensioned at least 3*n+15.
        in the program that calls ezfftb. the wsave array must be
        initialized by calling subroutine ezffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by ezfftf and ezfftb.


output parameters

r       if n is even define kmax=n/2
        if n is odd  define kmax=(n-1)/2

        then for i=1,...,n

             r(i)=azero plus the sum from k=1 to k=kmax of

             a(k)*cos(k*(i-1)*2*pi/n)+b(k)*sin(k*(i-1)*2*pi/n)

********************* complex notation **************************

        for j=1,...,n

        r(j) equals the sum from k=-kmax to k=kmax of

             c(k)*exp(i*k*(j-1)*2*pi/n)

        where

             c(k) = .5*cmplx(a(k),-b(k))   for k=1,...,kmax

             c(-k) = conjg(c(k))

             c(0) = azero

                  and i=sqrt(-1)

*************** amplitude - phase notation ***********************

        for i=1,...,n

        r(i) equals azero plus the sum from k=1 to k=kmax of

             alpha(k)*cos(k*(i-1)*2*pi/n+beta(k))

        where

             alpha(k) = sqrt(a(k)*a(k)+b(k)*b(k))

             cos(beta(k))=a(k)/alpha(k)

             sin(beta(k))=-b(k)/alpha(k)

******************************************************************

subroutine sinti(n,wsave)

******************************************************************

subroutine sinti initializes the array wsave which is used in
subroutine sint. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed.  the method
        is most efficient when n+1 is a product of small primes.

output parameter

wsave   a work array with at least int(2.5*n+15) locations.
        different wsave arrays are required for different values
        of n. the contents of wsave must not be changed between
        calls of sint.

******************************************************************

subroutine sint(n,x,wsave)

******************************************************************

subroutine sint computes the discrete fourier sine transform
of an odd sequence x(i). the transform is defined below at
output parameter x.

sint is the unnormalized inverse of itself since a call of sint
followed by another call of sint will multiply the input sequence
x by 2*(n+1).

the array wsave which is used by subroutine sint must be
initialized by calling subroutine sinti(n,wsave).

input parameters

n       the length of the sequence to be transformed.  the method
        is most efficient when n+1 is the product of small primes.

x       an array which contains the sequence to be transformed


wsave   a work array with dimension at least int(2.5*n+15)
        in the program that calls sint. the wsave array must be
        initialized by calling subroutine sinti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

             x(i)= the sum from k=1 to k=n

                  2*x(k)*sin(k*i*pi/(n+1))

             a call of sint followed by another call of
             sint will multiply the sequence x by 2*(n+1).
             hence sint is the unnormalized inverse
             of itself.

wsave   contains initialization calculations which must not be
        destroyed between calls of sint.

******************************************************************

subroutine costi(n,wsave)

******************************************************************

subroutine costi initializes the array wsave which is used in
subroutine cost. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed.  the method
        is most efficient when n-1 is a product of small primes.

output parameter

wsave   a work array which must be dimensioned at least 3*n+15.
        different wsave arrays are required for different values
        of n. the contents of wsave must not be changed between
        calls of cost.

******************************************************************

subroutine cost(n,x,wsave)

******************************************************************

subroutine cost computes the discrete fourier cosine transform
of an even sequence x(i). the transform is defined below at output
parameter x.

cost is the unnormalized inverse of itself since a call of cost
followed by another call of cost will multiply the input sequence
x by 2*(n-1). the transform is defined below at output parameter x

the array wsave which is used by subroutine cost must be
initialized by calling subroutine costi(n,wsave).

input parameters

n       the length of the sequence x. n must be greater than 1.
        the method is most efficient when n-1 is a product of
        small primes.

x       an array which contains the sequence to be transformed

wsave   a work array which must be dimensioned at least 3*n+15
        in the program that calls cost. the wsave array must be
        initialized by calling subroutine costi(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

            x(i) = x(1)+(-1)**(i-1)*x(n)

             + the sum from k=2 to k=n-1

                 2*x(k)*cos((k-1)*(i-1)*pi/(n-1))

             a call of cost followed by another call of
             cost will multiply the sequence x by 2*(n-1)
             hence cost is the unnormalized inverse
             of itself.

wsave   contains initialization calculations which must not be
        destroyed between calls of cost.

******************************************************************

subroutine sinqi(n,wsave)

******************************************************************

subroutine sinqi initializes the array wsave which is used in
both sinqf and sinqb. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed. the method
        is most efficient when n is a product of small primes.

output parameter

wsave   a work array which must be dimensioned at least 3*n+15.
        the same work array can be used for both sinqf and sinqb
        as long as n remains unchanged. different wsave arrays
        are required for different values of n. the contents of
        wsave must not be changed between calls of sinqf or sinqb.

******************************************************************

subroutine sinqf(n,x,wsave)

******************************************************************

subroutine sinqf computes the fast fourier transform of quarter
wave data. that is , sinqf computes the coefficients in a sine
series representation with only odd wave numbers. the transform
is defined below at output parameter x.

sinqb is the unnormalized inverse of sinqf since a call of sinqf
followed by a call of sinqb will multiply the input sequence x
by 4*n.

the array wsave which is used by subroutine sinqf must be
initialized by calling subroutine sinqi(n,wsave).


input parameters

n       the length of the array x to be transformed.  the method
        is most efficient when n is a product of small primes.

x       an array which contains the sequence to be transformed

wsave   a work array which must be dimensioned at least 3*n+15.
        in the program that calls sinqf. the wsave array must be
        initialized by calling subroutine sinqi(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

             x(i) = (-1)**(i-1)*x(n)

                + the sum from k=1 to k=n-1 of

                2*x(k)*sin((2*i-1)*k*pi/(2*n))

             a call of sinqf followed by a call of
             sinqb will multiply the sequence x by 4*n.
             therefore sinqb is the unnormalized inverse
             of sinqf.

wsave   contains initialization calculations which must not
        be destroyed between calls of sinqf or sinqb.

******************************************************************

subroutine sinqb(n,x,wsave)

******************************************************************

subroutine sinqb computes the fast fourier transform of quarter
wave data. that is , sinqb computes a sequence from its
representation in terms of a sine series with odd wave numbers.
the transform is defined below at output parameter x.

sinqf is the unnormalized inverse of sinqb since a call of sinqb
followed by a call of sinqf will multiply the input sequence x
by 4*n.

the array wsave which is used by subroutine sinqb must be
initialized by calling subroutine sinqi(n,wsave).


input parameters

n       the length of the array x to be transformed.  the method
        is most efficient when n is a product of small primes.

x       an array which contains the sequence to be transformed

wsave   a work array which must be dimensioned at least 3*n+15.
        in the program that calls sinqb. the wsave array must be
        initialized by calling subroutine sinqi(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

             x(i)= the sum from k=1 to k=n of

               4*x(k)*sin((2k-1)*i*pi/(2*n))

             a call of sinqb followed by a call of
             sinqf will multiply the sequence x by 4*n.
             therefore sinqf is the unnormalized inverse
             of sinqb.

wsave   contains initialization calculations which must not
        be destroyed between calls of sinqb or sinqf.

******************************************************************

subroutine cosqi(n,wsave)

******************************************************************

subroutine cosqi initializes the array wsave which is used in
both cosqf and cosqb. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the array to be transformed.  the method
        is most efficient when n is a product of small primes.

output parameter

wsave   a work array which must be dimensioned at least 3*n+15.
        the same work array can be used for both cosqf and cosqb
        as long as n remains unchanged. different wsave arrays
        are required for different values of n. the contents of
        wsave must not be changed between calls of cosqf or cosqb.

******************************************************************

subroutine cosqf(n,x,wsave)

******************************************************************

subroutine cosqf computes the fast fourier transform of quarter
wave data. that is , cosqf computes the coefficients in a cosine
series representation with only odd wave numbers. the transform
is defined below at output parameter x

cosqf is the unnormalized inverse of cosqb since a call of cosqf
followed by a call of cosqb will multiply the input sequence x
by 4*n.

the array wsave which is used by subroutine cosqf must be
initialized by calling subroutine cosqi(n,wsave).


input parameters

n       the length of the array x to be transformed.  the method
        is most efficient when n is a product of small primes.

x       an array which contains the sequence to be transformed

wsave   a work array which must be dimensioned at least 3*n+15
        in the program that calls cosqf. the wsave array must be
        initialized by calling subroutine cosqi(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

             x(i) = x(1) plus the sum from k=2 to k=n of

                2*x(k)*cos((2*i-1)*(k-1)*pi/(2*n))

             a call of cosqf followed by a call of
             cosqb will multiply the sequence x by 4*n.
             therefore cosqb is the unnormalized inverse
             of cosqf.

wsave   contains initialization calculations which must not
        be destroyed between calls of cosqf or cosqb.

******************************************************************

subroutine cosqb(n,x,wsave)

******************************************************************

subroutine cosqb computes the fast fourier transform of quarter
wave data. that is , cosqb computes a sequence from its
representation in terms of a cosine series with odd wave numbers.
the transform is defined below at output parameter x.

cosqb is the unnormalized inverse of cosqf since a call of cosqb
followed by a call of cosqf will multiply the input sequence x
by 4*n.

the array wsave which is used by subroutine cosqb must be
initialized by calling subroutine cosqi(n,wsave).


input parameters

n       the length of the array x to be transformed.  the method
        is most efficient when n is a product of small primes.

x       an array which contains the sequence to be transformed

wsave   a work array that must be dimensioned at least 3*n+15
        in the program that calls cosqb. the wsave array must be
        initialized by calling subroutine cosqi(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.

output parameters

x       for i=1,...,n

             x(i)= the sum from k=1 to k=n of

               4*x(k)*cos((2*k-1)*(i-1)*pi/(2*n))

             a call of cosqb followed by a call of
             cosqf will multiply the sequence x by 4*n.
             therefore cosqf is the unnormalized inverse
             of cosqb.

wsave   contains initialization calculations which must not
        be destroyed between calls of cosqb or cosqf.

******************************************************************

subroutine cffti(n,wsave)

******************************************************************

subroutine cffti initializes the array wsave which is used in
both cfftf and cfftb. the prime factorization of n together with
a tabulation of the trigonometric functions are computed and
stored in wsave.

input parameter

n       the length of the sequence to be transformed

output parameter

wsave   a work array which must be dimensioned at least 4*n+15
        the same work array can be used for both cfftf and cfftb
        as long as n remains unchanged. different wsave arrays
        are required for different values of n. the contents of
        wsave must not be changed between calls of cfftf or cfftb.

******************************************************************

subroutine cfftf(n,c,wsave)

******************************************************************

subroutine cfftf computes the forward complex discrete fourier
transform (the fourier analysis). equivalently , cfftf computes
the fourier coefficients of a complex periodic sequence.
the transform is defined below at output parameter c.

the transform is not normalized. to obtain a normalized transform
the output must be divided by n. otherwise a call of cfftf
followed by a call of cfftb will multiply the sequence by n.

the array wsave which is used by subroutine cfftf must be
initialized by calling subroutine cffti(n,wsave).

input parameters


n      the length of the complex sequence c. the method is
       more efficient when n is the product of small primes. n

c      a complex array of length n which contains the sequence

wsave   a real work array which must be dimensioned at least 4n+15
        in the program that calls cfftf. the wsave array must be
        initialized by calling subroutine cffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by cfftf and cfftb.

output parameters

c      for j=1,...,n

           c(j)=the sum from k=1,...,n of

                 c(k)*exp(-i*(j-1)*(k-1)*2*pi/n)

                       where i=sqrt(-1)

wsave   contains initialization calculations which must not be
        destroyed between calls of subroutine cfftf or cfftb

******************************************************************

subroutine cfftb(n,c,wsave)

******************************************************************

subroutine cfftb computes the backward complex discrete fourier
transform (the fourier synthesis). equivalently , cfftb computes
a complex periodic sequence from its fourier coefficients.
the transform is defined below at output parameter c.

a call of cfftf followed by a call of cfftb will multiply the
sequence by n.

the array wsave which is used by subroutine cfftb must be
initialized by calling subroutine cffti(n,wsave).

input parameters


n      the length of the complex sequence c. the method is
       more efficient when n is the product of small primes.

c      a complex array of length n which contains the sequence

wsave   a real work array which must be dimensioned at least 4n+15
        in the program that calls cfftb. the wsave array must be
        initialized by calling subroutine cffti(n,wsave) and a
        different wsave array must be used for each different
        value of n. this initialization does not have to be
        repeated so long as n remains unchanged thus subsequent
        transforms can be obtained faster than the first.
        the same wsave array can be used by cfftf and cfftb.

output parameters

c      for j=1,...,n

           c(j)=the sum from k=1,...,n of

                 c(k)*exp(i*(j-1)*(k-1)*2*pi/n)

                       where i=sqrt(-1)

wsave   contains initialization calculations which must not be
        destroyed between calls of subroutine cfftf or cfftb



["send index for vfftpk" describes a vectorized version of fftpack]

*/
