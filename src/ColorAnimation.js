import { hover } from '@testing-library/user-event/dist/hover';
import React from 'react';
import { ChangeColorButton } from './EditColorAnimation';


class ColorAnimation extends React.Component{
    constructor(props){
        super(props);
        this.animationState = 'inactive';
        this.mode = '';
        this.colorIndex = 0;
    }
    
    state = {
        length: '100',
        currentColor: {r:'100',g:'20',b:'200',a:'1'},
    };

        // quanify the distance between two colors 
    // using 4D distance formula
    compareColors(c1,c2){

        const x1 = c1.r;
        const x2 = c2.r;

        const y1= c1.g;
        const y2 = c2.g;

        const z1 = c1.b;
        const z2 = c2.b;

        const a1 = c1.a * 255;
        const a2 = c2.a * 255;


        const distance = Math.sqrt(
            Math.pow((x2 - x1), 2) +
            Math.pow((y2 - y1), 2) +
            Math.pow((z2 - z1), 2) +
            Math.pow((a2 - a1), 2)
        );

        return distance;
    }

    getRGB(colorObject){
        return colorObject.state.color;
    }

    runAnimation = () => {
        const tempColors = Object.values(this.props.colors);
        console.log("list: ",tempColors);
    

        // only run if  there are less than two colors
        if(tempColors.length < 2){
            return;
        }



        var startColorState,targetColorState;
        // restart if at the end of the color list
        if(this.colorIndex >= tempColors.length-1){
            startColorState = (tempColors[this.colorIndex]);
            targetColorState = (tempColors[0]);
            this.colorIndex = 0;
        }else{
            startColorState = (tempColors[this.colorIndex]);
            targetColorState = (tempColors[this.colorIndex+1]);
            this.colorIndex++;
        }

        const iterationCount = startColorState.transitionTime;
        const lingerTickCount = startColorState.lingerTime;
        const startColor = startColorState.color;
        const targetColor = targetColorState.color;

        console.log("startColor:", startColorState); console.log("targetColor:", targetColorState);console.log("iterationCount", iterationCount);

   
        // smooth transition from start to end color
        const dR = (targetColor.r - startColor.r)/iterationCount;
        const dG = (targetColor.g - startColor.g)/iterationCount;
        const dB = (targetColor.b - startColor.b)/iterationCount;
        const dA = (targetColor.a - startColor.a)/iterationCount;
        const changeData = {dR: dR, dG: dG, dB: dB, dA: dA};

        this.animationState = 'active';
        this.state.mode = 'shift';

        this.animationTick(startColor,targetColor,changeData,iterationCount,lingerTickCount);
    
    

    }

    
    // Async update one tick of whatever the current animation style is
    animationTick(oldColor,targetColor,changeData,ticksRemaning,lingerTicks){
        
        console.log("oldColor:", oldColor); console.log("targetColor:", targetColor); console.log("changeData:", changeData); console.log("ticksRemaning:", ticksRemaning);
        
        var newColor;
        if (this.animationState == 'inactive'){
            return;
        }
        // if there is still linger ticks, do nothing and go to next tick
        if (lingerTicks > 0) {
            setTimeout(() => {
                this.animationTick(oldColor, targetColor, changeData, ticksRemaning, lingerTicks - 1);
            }, 1);
            return;
        }

        if (ticksRemaning == 0){
            // transition to next phase if close enough to target color
            const distance = this.compareColors(oldColor, targetColor);
            if (distance < 2) {
                this.runAnimation();
            } else {
                setTimeout(() => {
                    this.animationTick(newColor,targetColor, changeData, ticksRemaning - 1,lingerTicks);
                }, 1);
            }
            return;
        }
        
        
        // if shift, slowly approach the target color each tick
        if (this.state.mode == 'shift'){
            newColor = {r:+oldColor.r + changeData.dR, g:+oldColor.g + changeData.dG, b:+oldColor.b + changeData.dB, a:+oldColor.a + changeData.dA,};
            this.setState({currentColor: newColor});
            setTimeout(() => {
                this.animationTick(newColor,targetColor,changeData,ticksRemaning-1);
            },1);
        }
    }

    render(){
        // preview of color animation
        this.block_style = {
            background: `rgba(${ this.state.currentColor.r }, ${ this.state.currentColor.g }, ${ this.state.currentColor.b }, ${ this.state.currentColor.a })`,
            width: "200px",
            height: "50px",
            cursor: "pointer",
        };

        return(<>
            <div style={this.block_style} onClick={this.runAnimation}>

            </div>
            {/* <button onClick={this.runAnimation}>test animation</button> */}
        
        </>)
    }



}



export default ColorAnimation;