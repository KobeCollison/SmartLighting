import React, { useState } from 'react';
import ColorAnimation from './ColorAnimation';
import { ChangeColorButton,ChangeTimeSlider } from './ColorUtils';
// import { PostSender } from './web_com';
import axios from 'axios';

const globalDefaultColor = {r:100,g:20,b:200,a:1};
const globalDefaultTime = 300;


function StyleSelect(){
    return (<>
    <select class="StyleSelect_button ">
            <option>Select Style</option>
            <option>Solid</option>
            <option>Shift</option>
            <option>Strobe</option>
            <option>Pulse</option>
            
    </select>
    </>);
}


class EditColorMenu extends React.Component{
  constructor(props){
    super(props);
    this.listContainer =  null;
    this.animationPreview = null;
    this.nextKeyValue = 1;
  }

  state = {
    colors: {},
    colorStates: [],
    listItems: [],
    len: '0',
    url: "",
    devices: [],
    renamePreview: "",
    connectedNodes: {},
    selectedDevice: "",
  }

  changeURL = this.changeURL.bind(this);
  changeRenamePreview = this.changeRenamePreview.bind(this);

  changeURL(event){
    this.setState({url : event.target.value}) 
  }
  changeRenamePreview(event){
    this.setState({renamePreview: event.target.value});
  }

  getNodes = () => {
    let addr = "http://kc_lumen.local/network/";
    axios.get(addr)
    .then((response) => {
        console.log("response",{response});
        if(response.status == 200){
            console.log("this",this);
            // devices is response.data mapped to have quotes removed
            let devices = Object.fromEntries(Object.entries(response.data).map(([key,value]) => [key.replace(/['"]+/g, ''),value]));
            console.log("devices",response.data);
            this.setState({connectedNodes: devices});
            console.log(this.state);
    
        }
        
    })
    .catch(function (error) {
        console.log(error);
    });
    console.log('state',this.state);
    //this.selectDevice(event);
}

  selectDevice = (event) => {
    // selected device is the connected node of the selected option's key property
    const selectedDevice = event.target.value;
    console.log("selected device",selectedDevice);
    this.setState({selectedDevice: selectedDevice});
  }


renameDevice = () => {
  // const url = this.state.url;
  const url = this.state.selectedDevice;
  const postAddress = "http://"+url+"/renameDevice/";
  const newName = this.state.renamePreview;

  let message = JSON.stringify({
    device: url,
    name: newName,
  });

  console.log("url:" ,postAddress);
  console.log("new Name:", newName);  

  axios.post(postAddress, message)
  .then(function (response) {
    console.log(response);
  })
  .catch(function (error) {
    console.log(error);
  });
}

  getColorStates = (data) =>{
    this.colors = data;
   }

  updateFeature = (key,color) => {
    console.log("key:", key);
    console.log("color:", color);
    const updatedColors = {...this.state.colors };
    updatedColors[key].color = color;

    this.setState({colors: updatedColors});
  }
 

  deleteColor = (index) => {
    console.log(index);
    var updatedColors = this.state.colors;
    delete updatedColors[index];
    this.setState({ colors: updatedColors });
    
  }

  

  // add a new color to the list, 
  // update the colors state so new edit button appears
  newColor = () =>{
    //append new blank color to the colors list state
    const newColorState = {color:globalDefaultColor,transitionTime:globalDefaultTime,lingerTime:globalDefaultTime};
    const index = this.state.colors.length;
    const key = this.nextKeyValue;
    this.nextKeyValue++;
    console.log("uf",this.updateFeature);


    // udated the colors dictionary 
    const updatedColors = {...this.state.colors };
    updatedColors[key] = newColorState;

    this.setState({colors: updatedColors});
    


    setTimeout(() => {
      console.log("colors: ",this.state.colors);
      console.log("list: ",Object.values(this.state.colors));
    }, 1);
    
  }

  changeColor = (index,newColor) => {
    const updatedColors = [...this.state.colors];
    updatedColors[index].color = newColor;
    this.setState({color: updatedColors});
  }


  // send the current color animatin settings to the current URL
  sendMessage(){

    // get url from input
    // const url = "http://192.168.1.170";
    const url = this.state.selectedDevice;
    const postAddress = url+"/colorChange/";

    // send string of color data as message
    const msg = JSON.stringify(this.state.colors);

    console.log("url:", postAddress);
    console.log("message:", msg);

    // POST request msg to url
    axios.post(postAddress,msg, {
        firstName: 'Fred',
        lastName: 'Flintstone'
    })
    .then(function (response) {
        console.log(response);
    })
    .catch(function (error) {
        console.log(error);
    });
}


x
  render(){
    // get list of available devices
    let devices = Object.entries(this.state.connectedNodes);
        let deviceListContainer = devices.map(([key,value,index]) => (
            <option key={key} value={key} onChange={this.selectDevice}>
                {this.state.connectedNodes[key]["name"]}
            </option>
        ));



    // get list of color options
    const colorVals = Object.entries(this.state.colors);
    const colorListContainer = colorVals.map(([key,value,index]) => (
      <li key={key}>
          Color {index} 
          <span>
          <ChangeColorButton defaultColor={globalDefaultColor} colors={this.state.colors} rowkey={+key} changeHandler={this.updateFeature}/>   
          <ChangeTimeSlider colors={this.state.colors} rowkey={key}/>
          <button onClick={() => {this.deleteColor(key);}} >x</button>
        </span>
      </li>
    ))


    this.animationPreview = <ColorAnimation colors={this.state.colors} onClick = {() => {
      this.sendAnimationData()
    }} />;

    return(<>
      <select onChange={this.selectDevice}>{deviceListContainer}</select>
      <button onClick={this.getNodes} >get network</button>
      <br/>

      <input type="text" onChange={this.changeRenamePreview}></input> 
      <button onClick={this.renameDevice} >rename device</button> <br/>
      
      <button onClick={() => this.sendMessage()} >apply changes</button> <br/>

      {colorListContainer}
      <button onClick={this.newColor}>+</button>
      {this.animationPreview}
    </>)
  }
}


  export {
    StyleSelect,
    EditColorMenu,
  }
  
