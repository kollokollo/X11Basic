//
// X11-Basic Logo mit POV-Ray 3.0
// urspruenglich adaptiert von ELSA Logo Joachim Keil, 1.11.97
//
// 13-Apr-1999	H. Bongartz		Pfeile jetzt zylindrisch
//
 
#include "colors.inc"

// #include "skies.inc"
// sky_sphere { S_Cloud5 }


// Parameter des Logos
//
#declare tiefe = 0.3;
#declare pfeilanfang = 0.0;
#declare pfeilradius = 0.045;
#declare pfeilbasis = 0.09;
#declare pfeillen1 = 1+pfeilradius; // Verlaengerung wg. Aufsetzen, s.u.
#declare pfeillen2 = 1.1;
#declare pfeillen3 = 1.3;
#declare pfeillen4 = 1.65;

#declare ipunktx = 0.7
#declare ipunkty = 0.71+pfeilradius

// Parameter der Darstellung
//
#declare diffpara = 0.6;
#declare ambipara = 0.3;

// Hintergrundlicht weiﬂ
global_settings { ambient_light rgb<1,1,1> }

// Position der Kamera
//
/*
camera {
	location <1,1,-5>  // -6 default
  	look_at  <1,1,0>
  	angle 40
}
*/

camera {
//	location <5,1,-6+2*clock>
	location <1+6*sin(clock/100*2*pi), 0.5, -2.9-6*cos(clock/100*2*pi)>
  	look_at  <ipunktx+1.1,1,tiefe/2>
  	angle 35
}


// Position der Lichtquelle
//
light_source { <0, 4,-5> White }
light_source { <-50*sin(clock/100*2*pi), 30,-15*cos(clock/100*2*pi)> White }


// Definition der Grundflaeche
plane { y,-0.01
	pigment { color rgb<.60,.60,1> }
        finish { ambient rgb<0.5, 0.5, 0.5> }
}

// Text mit X11-Basic
text { ttf "arial.ttf" "X11-Basic" tiefe, 0
	pigment { color Yellow }
  	finish { ambient 0.3 diffuse 0.6 } 
}


// Hintergrund-Box
box {
        <-10, -1, 6>
        <10, 10, 6.2>
        finish { ambient  0.3 diffuse 0.6}
        pigment {
              rgb<.50,.50,1>  
        }
}


union {

	// 3/4-tel Kreis als Toroid	
	difference {
		torus { 0.7, pfeilradius
			rotate -90*x
			translate <0, 0.73, 0.5 * tiefe>
		}
		box {
			<0, -0.1, -0.002>
			<1, 0.73, tiefe + 0.002>
		}
	}        
	

	// Der erste Pfeil
	union {	                       
		#declare pl = pfeillen1
		// Die Pfeilspitze
		cone {
			<0, pfeilanfang+pl, 0>, pfeilbasis
			<0, pfeilanfang+pl+0.2, 0>, 0
		}
		// Der Schaft dazu
		cylinder {
			<0, pfeilanfang, 0>
			<0, pfeilanfang+pl, 0>
			pfeilradius
		}
		translate <ipunktx, ipunkty, tiefe/2>
		translate -pfeilradius*y	// auf das "I" aufsetzen
	}
	
	// Der zweite Pfeil
	union {	                       
		#declare pl = pfeillen2
		// Die Pfeilspitze
		cone {
			<0, pfeilanfang+pl, 0>, pfeilbasis
			<0, pfeilanfang+pl+0.2, 0>, 0
		}
		// Der Schaft dazu
		cylinder {
			<0, pfeilanfang, 0>
			<0, pfeilanfang+pl, 0>
			pfeilradius
		}
		rotate -30*z
		translate <ipunktx, ipunkty, tiefe/2>
	}
	
	// Der dritte Pfeil
	union {
		#declare pl = pfeillen3
		// Die Pfeilspitze
		cone {
			<0, pfeilanfang+pl, 0>, pfeilbasis
			<0, pfeilanfang+pl+0.2, 0>, 0
		}
		// Der Schaft dazu
		cylinder {
			<0, pfeilanfang, 0>
			<0, pfeilanfang+pl, 0>
			pfeilradius
		}
		rotate -60*z
		translate <ipunktx, ipunkty, tiefe/2>
	}
	
	// Der vierte Pfeil
	union {
		#declare pl = pfeillen4
		// Die Pfeilspitze
		cone {
			<0, pfeilanfang+pl, 0>, pfeilbasis
			<0, pfeilanfang+pl+0.2, 0>, 0
		}
		// Der Schaft dazu
		cylinder {
			<0, pfeilanfang, 0>
			<0, pfeilanfang+pl, 0>
			pfeilradius
		}
		rotate -90*z
		translate <ipunktx, ipunkty, tiefe/2>
	}

 	pigment { color Blue }
  	finish { phong 1 }
  	finish { ambient ambipara diffuse diffpara } 
}
