import React, { useState } from 'react';
import reactCSS from 'reactcss'
import { ChromePicker } from 'react-color';


const globalDefaultColor = {r:100,g:20,b:200,a:1};
const globalDefaultTime = 300;


class ChangeColorButton extends React.Component {
    constructor(props){
      super(props);
      this.currentColor = this.props.colors[this.props.rowkey].color;
      
      this.updateList = this.props.changeHandler;
    }

    state = {
      displayColorPicker: false,
      color: this.props.colors[this.props.rowkey].color
    };
    
    

    handleClick = () => {
      this.setState({ displayColorPicker: !this.state.displayColorPicker })
    };
  
    handleClose = () => {
      this.setState({ displayColorPicker: false })
    };
  
    handleChange = (color) => {
      this.setState({ color: color.rgb })
      this.updateList(this.props.rowkey,this.state.color);
    };
  
    render() {
      const styles = reactCSS({
        'default': {
          color: {
            background: `rgba(${ this.state.color.r }, ${ this.state.color.g }, ${ this.state.color.b }, ${ this.state.color.a })`,
            borderRadius: '50%',
            height: '2.5em',
            width: '2.5em',
            boxShadow: '0 0 0 1px rgba(0,0,0,.1)',
            cursor: 'pointer',
            display: 'inline',
          },

          popover: {
            position: 'absolute',
            zIndex: '2',
          },
          cover: {
            position: 'fixed',
            top: '0px',
            right: '0px',
            bottom: '0px',
            left: '0px',
          },
          inline: {
            display: 'inline',
            margin: "5px",
          }
        },
      });
  
      return (
        <div style={styles.inline}>
            <button onClick={ this.handleClick } style={ styles.color }/>
          { this.state.displayColorPicker ? <div style={ styles.popover }>
            <div style={ styles.cover } onClick={ this.handleClose }/>
            <ChromePicker  color={this.state.color} onChange={ this.handleChange } />
          </div> : null }
            {this.animation}
        </div>
      )
    }
  }

  class ChangeTimeSlider extends React.Component{
    constructor(props){
      super(props);
    }

    state = {
      transitionValue: globalDefaultTime,
      lingerValue: globalDefaultTime,
    }

    handleTransitionTimeChange = (event) => {
      this.setState({ transitionValue: +event.target.value });
      this.props.colors[this.props.rowkey].transitionTime = +event.target.value;
      console.log(this.props.colors);
    };

    handleLingerTimeChange = (event) => {
      this.setState({ lingerValue: +event.target.value });
      this.props.colors[this.props.rowkey].lingerTime = +event.target.value;
      console.log(this.props.colors);
    };


    render(){
      console.log("props" ,this.props);
      return <div>
        <input type="range" min="50" max="1000" defaultValue={globalDefaultTime} value={this.state.transitionValue} class="slider" onInput={this.handleTransitionTimeChange} /> 
        <input type="number"  defaultValue={globalDefaultTime}  value={this.state.transitionValue} style={{width: '4em'}} onInput={this.handleTransitionTimeChange} /> <br />

        <input type="range" min="50" max="1000" defaultValue={globalDefaultTime}  value={this.state.lingerValue} class="slider" onInput={this.handleLingerTimeChange} /> 
        <input type="number"  defaultValue={globalDefaultTime}   value={this.state.lingerValue}  style={{width: '4em'}} onInput={this.handleLingerTimeChange} /> <br />
      </div>
    }

  }


  export {ChangeColorButton,ChangeTimeSlider}