const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
let count=0;
const source=fs.readFileSync(__dirname+'/../src/webUI/app.js','utf8').replace(/\r\n/g,'\n');
new vm.Script(source);count++;
const marker='if (!isDataValuePresent(value)) {\n          return null;';
assert(source.includes(marker));count++;
let regressionCaught=false;
try {new vm.Script(source.replace(marker,'if (!isDataValuePresent(value))\n          return null;'));} catch {regressionCaught=true;}
assert(regressionCaught);count++;
class Element {constructor(){this.children=[];this.style={};this.dataset={};this.textContent='';}appendChild(x){this.children.push(x);}replaceChildren(...xs){this.children=xs;}}
const nodes=new Map();
const document={addEventListener(){},getElementById(id){if(!nodes.has(id))nodes.set(id,new Element());return nodes.get(id);},createElement(){return new Element();},createDocumentFragment(){return new Element();}};
const ctx=vm.createContext({document,window:{addEventListener(){}},console});
vm.runInContext(source,ctx);
const run=expression=>vm.runInContext(expression,ctx);
for(let i=0;i<=1000;i++){
  const v=i/10,p=i/10;
  const data={protocol:'MODBUS_EASUN_SMS',LiveData:{PV_Input_Voltage:v,PV1_Input_Power:p,PV_Total_Input_Power:p,AC_Out_VA:353,AC_Out_Watt:144,AC_Out_Voltage:230,AC_Out_Current_L1:1.54,Battery_Voltage:48,Battery_Percent:48}};
  const before=JSON.stringify(data);ctx.testData=data;
  assert.equal(run('getSolarChannelInfo(testData).voltage'),v<10?0:v);
  assert.equal(run('getSolarChannelInfo(testData).power'),p<10?0:p);
  assert.equal(run('totalSolarPower(testData)'),p<10?0:p);
  assert.equal(run('displayLoadPower(testData)'),144);
  run('renderOverview(testData)');
  assert.equal(JSON.stringify(data),before,'UI must not mutate raw telemetry');count+=5;
}
for(const data of [{},{LiveData:{}},{protocol:'OTHER',LiveData:{AC_Out_VA:353,AC_Out_Watt:144}},{protocol:'MODBUS_EASUN_SMS',LiveData:{AC_Out_Watt:144}},{DeviceData:{Protocol_ID:'MODBUS_EASUN_SMS'},LiveData:{AC_Out_VA:353}},{EspData:{detect_protocol_name:'MODBUS_EASUN_SMS'},LiveData:{AC_Out_VA:353}}]){
 ctx.testData=data;run('renderOverview(testData)');count++;
}
ctx.testData={protocol:'OTHER',LiveData:{AC_Out_VA:353,AC_Out_Watt:144}};assert.equal(run('displayLoadPower(testData)'),144);count++;
ctx.testData={protocol:'MODBUS_EASUN_SMS',LiveData:{AC_Out_Watt:144}};assert.equal(run('displayLoadPower(testData)'),144);count++;
ctx.testData={LiveData:{PV1_Input_Power:9.9}};assert.equal(run('totalSolarPower(testData)'),0);count++;
ctx.testData={LiveData:{PV1_Input_Power:10}};assert.equal(run('totalSolarPower(testData)'),10);count++;
// Regression: a VA-only sample must never appear as watts; zero W is valid.
for(const protocol of ['MODBUS_EASUN_SMS','OTHER']){
 for(const live of [{AC_Out_Watt:176,AC_Out_VA:398},{AC_Out_Watt:0,AC_Out_VA:111},{AC_Out_VA:400},{AC_Output_Active_Power:168,AC_Out_VA:400}]){
  ctx.testData={protocol,LiveData:live};
  assert.equal(run('displayLoadPower(testData)'),live.AC_Out_Watt ?? live.AC_Output_Active_Power ?? null);count++;
 }
}
ctx.testData={protocol:'MODBUS_EASUN_SMS',LiveData:{AC_Out_Watt:176,AC_Out_VA:398}};
run('renderOverview(testData)');
function treeText(node){return [node.textContent,...node.children.map(treeText)].join(' ');}
const rendered=treeText(nodes.get('overviewRows'));
assert(rendered.includes('176 W'));assert(rendered.includes('398 VA'));assert(!rendered.includes('398 W'));count+=3;
console.log(`PASS ${count} assertions, including syntax regression, threshold boundaries, 1001 dashboard renders, raw telemetry preservation and separate W/VA output.`);
