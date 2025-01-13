import React from "react";
import axios from "axios";

class PostSender extends React.Component{
    constructor(props){
        super(props);
        this.url = "";
      }
      
    state = {
        url: "",
        message: "",
    }

    changeURL = this.changeURL.bind(this);
    changeMessage = this.changeMessage.bind(this);
    sendMessage = this.sendMessage.bind(this);

    changeURL(event){
        this.setState({url : event.target.value})
    }

    changeMessage(event){
        this.setState({message : event.target.value})
    }
 
    sendMessage(colorPacket){
        const url = this.state.url;
        const msg = this.state.message;


        console.log("url:", url);
        console.log("message:", msg);

        axios.post(url,msg, {
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

     
    render(){
        return <> 
        <span> <input type="text" onChange={this.changeURL}></input></span>
         
        <span>message: <textarea  onChange={this.changeMessage}></textarea></span>
        <button onClick={this.sendMessage} >send</button>
         </>
    }
}

class NodePicker extends React.Component{
    constructor(props){
        super(props);  
    }
    state = {
        connectedNodes: {},
    }
    

    getNodes = () => {
        let addr = "http://kc_lumen.local/network/";
        axios.get(addr)
        .then((response) => {
            console.log("response",{response});
            if(response.status == 200){
                console.log("this",this);
                this.setState({connectedNodes: response.data});
                console.log(this.state);
        
            }
            
        })
        .catch(function (error) {
            console.log(error);
        });
        console.log('state',this.state);
    }


    render(){
        let devices = Object.entries(this.state.connectedNodes);
        let listContainer = devices.map(([key,value,index]) => (
            <option key={index}>
                {key}
            </option>
        ));
        return <> 
        <select >{listContainer}</select>
        <button onClick={this.getNodes} >network</button>
         </>
    }
}


export {PostSender,NodePicker}