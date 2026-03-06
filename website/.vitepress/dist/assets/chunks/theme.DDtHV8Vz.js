const __vite__mapDeps=(i,m=__vite__mapDeps,d=(m.f||(m.f=["assets/chunks/VPAlgoliaSearchBox.CPS0RSuv.js","assets/chunks/framework.ZxETiGz0.js","assets/chunks/index.BPH623Bm.js","assets/chunks/index.tqIctuLa.js","assets/chunks/index.CyGSWCqK.js","assets/chunks/index.DGTKRUWM.js","assets/chunks/index.pqLJMZUs.js","assets/chunks/index.D9sbPGjv.js"])))=>i.map(i=>d[i]);
import{d as w,c as l,r as x,n as N,o as i,a as Q,t as v,b as A,w as y,T as se,e as g,_ as q,u as Bt,i as Nt,f as Ft,g as Xe,h as O,j as t,k as u,l as xe,m as je,p as C,q as ne,s as Ae,v as ee,x as de,y as Ze,z as Rt,A as Ht,F,B as j,C as ke,D as De,E,G as ht,H as ce,I as ft,J as Be,K as ue,L as Ne,M as zt,N as W,O as Ut,P as Pe,Q as mt,R as Fe,S as Ee,U as Gt,V as Z,W as gt,X as xt,Y as jt,Z as Jt,$ as Qt,a0 as Te,a1 as Wt,a2 as Re,a3 as et,a4 as Kt,a5 as Yt,a6 as kt,a7 as Xt,a8 as Zt}from"./framework.ZxETiGz0.js";const en=w({__name:"VPBadge",props:{text:{},type:{default:"tip"}},setup(e){return(n,s)=>(i(),l("span",{class:N(["VPBadge",e.type])},[x(n.$slots,"default",{},()=>[Q(v(e.text),1)])],2))}}),tn={key:0,class:"VPBackdrop"},nn=w({__name:"VPBackdrop",props:{show:{type:Boolean}},setup(e){return(n,s)=>(i(),A(se,{name:"fade"},{default:y(()=>[e.show?(i(),l("div",tn)):g("",!0)]),_:1}))}}),sn=q(nn,[["__scopeId","data-v-c79a1216"]]),U=Bt;function on(e,n){let s,a=!1;return()=>{s&&clearTimeout(s),a?s=setTimeout(e,n):(e(),(a=!0)&&setTimeout(()=>a=!1,n))}}function Je(e){return e.startsWith("/")?e:`/${e}`}function tt(e){const{pathname:n,search:s,hash:a,protocol:o}=new URL(e,"http://a.com");if(Nt(e)||e.startsWith("#")||!o.startsWith("http")||!Ft(n))return e;const{site:c}=U(),r=n.endsWith("/")||n.endsWith(".html")?e:e.replace(/(?:(^\.+)\/)?.*$/,`$1${n.replace(/(\.md)?$/,c.value.cleanUrls?"":".html")}${s}${a}`);return Xe(r)}function Oe({correspondingLink:e=!1}={}){const{site:n,localeIndex:s,page:a,theme:o,hash:c}=U(),r=O(()=>{var _,p;return{label:(_=n.value.locales[s.value])==null?void 0:_.label,link:((p=n.value.locales[s.value])==null?void 0:p.link)||(s.value==="root"?"/":`/${s.value}/`)}});return{localeLinks:O(()=>Object.entries(n.value.locales).flatMap(([_,p])=>r.value.label===p.label?[]:{text:p.label,link:an(p.link||(_==="root"?"/":`/${_}/`),o.value.i18nRouting!==!1&&e,a.value.relativePath.slice(r.value.link.length-1),!n.value.cleanUrls)+c.value})),currentLang:r}}function an(e,n,s,a){return n?e.replace(/\/$/,"")+Je(s.replace(/(^|\/)index\.md$/,"$1").replace(/\.md$/,a?".html":"")):e}const ln={class:"NotFound"},cn={class:"code"},rn={class:"title"},dn={class:"quote"},_n={class:"action"},un=["href","aria-label"],pn=w({__name:"NotFound",setup(e){const{theme:n}=U(),{currentLang:s}=Oe();return(a,o)=>{var c,r,d,_,p;return i(),l("div",ln,[t("p",cn,v(((c=u(n).notFound)==null?void 0:c.code)??"404"),1),t("h1",rn,v(((r=u(n).notFound)==null?void 0:r.title)??"PAGE NOT FOUND"),1),o[0]||(o[0]=t("div",{class:"divider"},null,-1)),t("blockquote",dn,v(((d=u(n).notFound)==null?void 0:d.quote)??"But if you don't change your direction, and if you keep looking, you may end up where you are heading."),1),t("div",_n,[t("a",{class:"link",href:u(Xe)(u(s).link),"aria-label":((_=u(n).notFound)==null?void 0:_.linkLabel)??"go to home"},v(((p=u(n).notFound)==null?void 0:p.linkText)??"Take me home"),9,un)])])}}}),vn=q(pn,[["__scopeId","data-v-d6be1790"]]);function bt(e,n){if(Array.isArray(e))return qe(e);if(e==null)return[];n=Je(n);const s=Object.keys(e).sort((o,c)=>c.split("/").length-o.split("/").length).find(o=>n.startsWith(Je(o))),a=s?e[s]:[];return Array.isArray(a)?qe(a):qe(a.items,a.base)}function hn(e){const n=[];let s=0;for(const a in e){const o=e[a];if(o.items){s=n.push(o);continue}n[s]||n.push({items:[]}),n[s].items.push(o)}return n}function fn(e){const n=[];function s(a){for(const o of a)o.text&&o.link&&n.push({text:o.text,link:o.link,docFooterText:o.docFooterText}),o.items&&s(o.items)}return s(e),n}function Qe(e,n){return Array.isArray(n)?n.some(s=>Qe(e,s)):xe(e,n.link)?!0:n.items?Qe(e,n.items):!1}function qe(e,n){return[...e].map(s=>{const a={...s},o=a.base||n;return o&&a.link&&(a.link=o+a.link),a.items&&(a.items=qe(a.items,o)),a})}function _e(){const{frontmatter:e,page:n,theme:s}=U(),a=je("(min-width: 960px)"),o=C(!1),c=O(()=>{const I=s.value.sidebar,M=n.value.relativePath;return I?bt(I,M):[]}),r=C(c.value);ne(c,(I,M)=>{JSON.stringify(I)!==JSON.stringify(M)&&(r.value=c.value)});const d=O(()=>e.value.sidebar!==!1&&r.value.length>0&&e.value.layout!=="home"),_=O(()=>p?e.value.aside==null?s.value.aside==="left":e.value.aside==="left":!1),p=O(()=>e.value.layout==="home"?!1:e.value.aside!=null?!!e.value.aside:s.value.aside!==!1),m=O(()=>d.value&&a.value),h=O(()=>d.value?hn(r.value):[]);function S(){o.value=!0}function f(){o.value=!1}function b(){o.value?f():S()}return{isOpen:o,sidebar:r,sidebarGroups:h,hasSidebar:d,hasAside:p,leftAside:_,isSidebarEnabled:m,open:S,close:f,toggle:b}}function mn(e,n){let s;Ae(()=>{s=e.value?document.activeElement:void 0}),ee(()=>{window.addEventListener("keyup",a)}),de(()=>{window.removeEventListener("keyup",a)});function a(o){o.key==="Escape"&&e.value&&(n(),s==null||s.focus())}}function gn(e){const{page:n,hash:s}=U(),a=C(!1),o=O(()=>e.value.collapsed!=null),c=O(()=>!!e.value.link),r=C(!1),d=()=>{r.value=xe(n.value.relativePath,e.value.link)};ne([n,e,s],d),ee(d);const _=O(()=>r.value?!0:e.value.items?Qe(n.value.relativePath,e.value.items):!1),p=O(()=>!!(e.value.items&&e.value.items.length));Ae(()=>{a.value=!!(o.value&&e.value.collapsed)}),Ze(()=>{(r.value||_.value)&&(a.value=!1)});function m(){o.value&&(a.value=!a.value)}return{collapsed:a,collapsible:o,isLink:c,isActiveLink:r,hasActiveLink:_,hasChildren:p,toggle:m}}function xn(){const{hasSidebar:e}=_e(),n=je("(min-width: 960px)"),s=je("(min-width: 1280px)");return{isAsideEnabled:O(()=>!s.value&&!n.value?!1:e.value?s.value:n.value)}}const kn=/\b(?:VPBadge|header-anchor|footnote-ref|ignore-header)\b/,We=[];function yt(e){return typeof e.outline=="object"&&!Array.isArray(e.outline)&&e.outline.label||e.outlineTitle||"On this page"}function nt(e){const n=[...document.querySelectorAll(".VPDoc :where(h1,h2,h3,h4,h5,h6)")].filter(s=>s.id&&s.hasChildNodes()).map(s=>{const a=Number(s.tagName[1]);return{element:s,title:bn(s),link:"#"+s.id,level:a}});return yn(n,e)}function bn(e){let n="";for(const s of e.childNodes)if(s.nodeType===1){if(kn.test(s.className))continue;n+=s.textContent}else s.nodeType===3&&(n+=s.textContent);return n.trim()}function yn(e,n){if(n===!1)return[];const s=(typeof n=="object"&&!Array.isArray(n)?n.level:n)||2,[a,o]=typeof s=="number"?[s,s]:s==="deep"?[2,6]:s;return Cn(e,a,o)}function $n(e,n){const{isAsideEnabled:s}=xn(),a=on(c,100);let o=null;ee(()=>{requestAnimationFrame(c),window.addEventListener("scroll",a)}),Rt(()=>{r(location.hash)}),de(()=>{window.removeEventListener("scroll",a)});function c(){if(!s.value)return;const d=window.scrollY,_=window.innerHeight,p=document.body.offsetHeight,m=Math.abs(d+_-p)<1,h=We.map(({element:f,link:b})=>({link:b,top:Sn(f)})).filter(({top:f})=>!Number.isNaN(f)).sort((f,b)=>f.top-b.top);if(!h.length){r(null);return}if(d<1){r(null);return}if(m){r(h[h.length-1].link);return}let S=null;for(const{link:f,top:b}of h){if(b>d+Ht()+4)break;S=f}r(S)}function r(d){o&&o.classList.remove("active"),d==null?o=null:o=e.value.querySelector(`a[href="${decodeURIComponent(d)}"]`);const _=o;_?(_.classList.add("active"),n.value.style.top=_.offsetTop+39+"px",n.value.style.opacity="1"):(n.value.style.top="33px",n.value.style.opacity="0")}}function Sn(e){let n=0;for(;e!==document.body;){if(e===null)return NaN;n+=e.offsetTop,e=e.offsetParent}return n}function Cn(e,n,s){We.length=0;const a=[],o=[];return e.forEach(c=>{const r={...c,children:[]};let d=o[o.length-1];for(;d&&d.level>=r.level;)o.pop(),d=o[o.length-1];if(r.element.classList.contains("ignore-header")||d&&"shouldIgnore"in d){o.push({level:r.level,shouldIgnore:!0});return}r.level>s||r.level<n||(We.push({element:r.element,link:r.link}),d?d.children.push(r):a.push(r),o.push(r))}),a}const Ln=["href","title"],Pn=w({__name:"VPDocOutlineItem",props:{headers:{},root:{type:Boolean}},setup(e){function n({target:s}){const a=s.href.split("#")[1],o=document.getElementById(decodeURIComponent(a));o==null||o.focus({preventScroll:!0})}return(s,a)=>{const o=ke("VPDocOutlineItem",!0);return i(),l("ul",{class:N(["VPDocOutlineItem",e.root?"root":"nested"])},[(i(!0),l(F,null,j(e.headers,({children:c,link:r,title:d})=>(i(),l("li",null,[t("a",{class:"outline-link",href:r,onClick:n,title:d},v(d),9,Ln),c!=null&&c.length?(i(),A(o,{key:0,headers:c},null,8,["headers"])):g("",!0)]))),256))],2)}}}),$t=q(Pn,[["__scopeId","data-v-b933a997"]]),Tn={class:"content"},On={"aria-level":"2",class:"outline-title",id:"doc-outline-aria-label",role:"heading"},In=w({__name:"VPDocAsideOutline",setup(e){const{frontmatter:n,theme:s}=U(),a=ht([]);De(()=>{a.value=nt(n.value.outline??s.value.outline)});const o=C(),c=C();return $n(o,c),(r,d)=>(i(),l("nav",{"aria-labelledby":"doc-outline-aria-label",class:N(["VPDocAsideOutline",{"has-outline":a.value.length>0}]),ref_key:"container",ref:o},[t("div",Tn,[t("div",{class:"outline-marker",ref_key:"marker",ref:c},null,512),t("div",On,v(u(yt)(u(s))),1),E($t,{headers:a.value,root:!0},null,8,["headers"])])],2))}}),wn=q(In,[["__scopeId","data-v-a5bbad30"]]),En={class:"VPDocAsideCarbonAds"},qn=w({__name:"VPDocAsideCarbonAds",props:{carbonAds:{}},setup(e){const n=()=>null;return(s,a)=>(i(),l("div",En,[E(u(n),{"carbon-ads":e.carbonAds},null,8,["carbon-ads"])]))}}),Vn={class:"VPDocAside"},Mn=w({__name:"VPDocAside",setup(e){const{theme:n}=U();return(s,a)=>(i(),l("div",Vn,[x(s.$slots,"aside-top",{},void 0,!0),x(s.$slots,"aside-outline-before",{},void 0,!0),E(wn),x(s.$slots,"aside-outline-after",{},void 0,!0),a[0]||(a[0]=t("div",{class:"spacer"},null,-1)),x(s.$slots,"aside-ads-before",{},void 0,!0),u(n).carbonAds?(i(),A(qn,{key:0,"carbon-ads":u(n).carbonAds},null,8,["carbon-ads"])):g("",!0),x(s.$slots,"aside-ads-after",{},void 0,!0),x(s.$slots,"aside-bottom",{},void 0,!0)]))}}),An=q(Mn,[["__scopeId","data-v-3f215769"]]);function Dn(){const{theme:e,page:n}=U();return O(()=>{const{text:s="Edit this page",pattern:a=""}=e.value.editLink||{};let o;return typeof a=="function"?o=a(n.value):o=a.replace(/:path/g,n.value.filePath),{url:o,text:s}})}function Bn(){const{page:e,theme:n,frontmatter:s}=U();return O(()=>{var p,m,h,S,f,b,I,M;const a=bt(n.value.sidebar,e.value.relativePath),o=fn(a),c=Nn(o,H=>H.link.replace(/[?#].*$/,"")),r=c.findIndex(H=>xe(e.value.relativePath,H.link)),d=((p=n.value.docFooter)==null?void 0:p.prev)===!1&&!s.value.prev||s.value.prev===!1,_=((m=n.value.docFooter)==null?void 0:m.next)===!1&&!s.value.next||s.value.next===!1;return{prev:d?void 0:{text:(typeof s.value.prev=="string"?s.value.prev:typeof s.value.prev=="object"?s.value.prev.text:void 0)??((h=c[r-1])==null?void 0:h.docFooterText)??((S=c[r-1])==null?void 0:S.text),link:(typeof s.value.prev=="object"?s.value.prev.link:void 0)??((f=c[r-1])==null?void 0:f.link)},next:_?void 0:{text:(typeof s.value.next=="string"?s.value.next:typeof s.value.next=="object"?s.value.next.text:void 0)??((b=c[r+1])==null?void 0:b.docFooterText)??((I=c[r+1])==null?void 0:I.text),link:(typeof s.value.next=="object"?s.value.next.link:void 0)??((M=c[r+1])==null?void 0:M.link)}}})}function Nn(e,n){const s=new Set;return e.filter(a=>{const o=n(a);return s.has(o)?!1:s.add(o)})}const re=w({__name:"VPLink",props:{tag:{},href:{},noIcon:{type:Boolean},target:{},rel:{}},setup(e){const n=e,s=O(()=>n.tag??(n.href?"a":"span")),a=O(()=>n.href&&ft.test(n.href)||n.target==="_blank");return(o,c)=>(i(),A(ce(s.value),{class:N(["VPLink",{link:e.href,"vp-external-link-icon":a.value,"no-icon":e.noIcon}]),href:e.href?u(tt)(e.href):void 0,target:e.target??(a.value?"_blank":void 0),rel:e.rel??(a.value?"noreferrer":void 0)},{default:y(()=>[x(o.$slots,"default")]),_:3},8,["class","href","target","rel"]))}}),Fn={class:"VPLastUpdated"},Rn=["datetime"],Hn=w({__name:"VPDocFooterLastUpdated",setup(e){const{theme:n,page:s,lang:a}=U(),o=O(()=>new Date(s.value.lastUpdated)),c=O(()=>o.value.toISOString()),r=C("");return ee(()=>{Ae(()=>{var d,_,p;r.value=new Intl.DateTimeFormat((_=(d=n.value.lastUpdated)==null?void 0:d.formatOptions)!=null&&_.forceLocale?a.value:void 0,((p=n.value.lastUpdated)==null?void 0:p.formatOptions)??{dateStyle:"short",timeStyle:"short"}).format(o.value)})}),(d,_)=>{var p;return i(),l("p",Fn,[Q(v(((p=u(n).lastUpdated)==null?void 0:p.text)||u(n).lastUpdatedText||"Last updated")+": ",1),t("time",{datetime:c.value},v(r.value),9,Rn)])}}}),zn=q(Hn,[["__scopeId","data-v-e98dd255"]]),Un={key:0,class:"VPDocFooter"},Gn={key:0,class:"edit-info"},jn={key:0,class:"edit-link"},Jn={key:1,class:"last-updated"},Qn={key:1,class:"prev-next","aria-labelledby":"doc-footer-aria-label"},Wn={class:"pager"},Kn=["innerHTML"],Yn=["innerHTML"],Xn={class:"pager"},Zn=["innerHTML"],es=["innerHTML"],ts=w({__name:"VPDocFooter",setup(e){const{theme:n,page:s,frontmatter:a}=U(),o=Dn(),c=Bn(),r=O(()=>n.value.editLink&&a.value.editLink!==!1),d=O(()=>s.value.lastUpdated),_=O(()=>r.value||d.value||c.value.prev||c.value.next);return(p,m)=>{var h,S,f,b;return _.value?(i(),l("footer",Un,[x(p.$slots,"doc-footer-before",{},void 0,!0),r.value||d.value?(i(),l("div",Gn,[r.value?(i(),l("div",jn,[E(re,{class:"edit-link-button",href:u(o).url,"no-icon":!0},{default:y(()=>[m[0]||(m[0]=t("span",{class:"vpi-square-pen edit-link-icon"},null,-1)),Q(" "+v(u(o).text),1)]),_:1},8,["href"])])):g("",!0),d.value?(i(),l("div",Jn,[E(zn)])):g("",!0)])):g("",!0),(h=u(c).prev)!=null&&h.link||(S=u(c).next)!=null&&S.link?(i(),l("nav",Qn,[m[1]||(m[1]=t("span",{class:"visually-hidden",id:"doc-footer-aria-label"},"Pager",-1)),t("div",Wn,[(f=u(c).prev)!=null&&f.link?(i(),A(re,{key:0,class:"pager-link prev",href:u(c).prev.link},{default:y(()=>{var I;return[t("span",{class:"desc",innerHTML:((I=u(n).docFooter)==null?void 0:I.prev)||"Previous page"},null,8,Kn),t("span",{class:"title",innerHTML:u(c).prev.text},null,8,Yn)]}),_:1},8,["href"])):g("",!0)]),t("div",Xn,[(b=u(c).next)!=null&&b.link?(i(),A(re,{key:0,class:"pager-link next",href:u(c).next.link},{default:y(()=>{var I;return[t("span",{class:"desc",innerHTML:((I=u(n).docFooter)==null?void 0:I.next)||"Next page"},null,8,Zn),t("span",{class:"title",innerHTML:u(c).next.text},null,8,es)]}),_:1},8,["href"])):g("",!0)])])):g("",!0)])):g("",!0)}}}),ns=q(ts,[["__scopeId","data-v-e257564d"]]),ss={class:"container"},os={class:"aside-container"},is={class:"aside-content"},as={class:"content"},ls={class:"content-container"},cs={class:"main"},rs=w({__name:"VPDoc",setup(e){const{theme:n}=U(),s=Be(),{hasSidebar:a,hasAside:o,leftAside:c}=_e(),r=O(()=>s.path.replace(/[./]+/g,"_").replace(/_html$/,""));return(d,_)=>{const p=ke("Content");return i(),l("div",{class:N(["VPDoc",{"has-sidebar":u(a),"has-aside":u(o)}])},[x(d.$slots,"doc-top",{},void 0,!0),t("div",ss,[u(o)?(i(),l("div",{key:0,class:N(["aside",{"left-aside":u(c)}])},[_[0]||(_[0]=t("div",{class:"aside-curtain"},null,-1)),t("div",os,[t("div",is,[E(An,null,{"aside-top":y(()=>[x(d.$slots,"aside-top",{},void 0,!0)]),"aside-bottom":y(()=>[x(d.$slots,"aside-bottom",{},void 0,!0)]),"aside-outline-before":y(()=>[x(d.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":y(()=>[x(d.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":y(()=>[x(d.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":y(()=>[x(d.$slots,"aside-ads-after",{},void 0,!0)]),_:3})])])],2)):g("",!0),t("div",as,[t("div",ls,[x(d.$slots,"doc-before",{},void 0,!0),t("main",cs,[E(p,{class:N(["vp-doc",[r.value,u(n).externalLinkIcon&&"external-link-icon-enabled"]])},null,8,["class"])]),E(ns,null,{"doc-footer-before":y(()=>[x(d.$slots,"doc-footer-before",{},void 0,!0)]),_:3}),x(d.$slots,"doc-after",{},void 0,!0)])])]),x(d.$slots,"doc-bottom",{},void 0,!0)],2)}}}),ds=q(rs,[["__scopeId","data-v-39a288b8"]]),_s=w({__name:"VPButton",props:{tag:{},size:{default:"medium"},theme:{default:"brand"},text:{},href:{},target:{},rel:{}},setup(e){const n=e,s=O(()=>n.href&&ft.test(n.href)),a=O(()=>n.tag||(n.href?"a":"button"));return(o,c)=>(i(),A(ce(a.value),{class:N(["VPButton",[e.size,e.theme]]),href:e.href?u(tt)(e.href):void 0,target:n.target??(s.value?"_blank":void 0),rel:n.rel??(s.value?"noreferrer":void 0)},{default:y(()=>[Q(v(e.text),1)]),_:1},8,["class","href","target","rel"]))}}),us=q(_s,[["__scopeId","data-v-fa7799d5"]]),ps=["src","alt"],vs=w({inheritAttrs:!1,__name:"VPImage",props:{image:{},alt:{}},setup(e){return(n,s)=>{const a=ke("VPImage",!0);return e.image?(i(),l(F,{key:0},[typeof e.image=="string"||"src"in e.image?(i(),l("img",ue({key:0,class:"VPImage"},typeof e.image=="string"?n.$attrs:{...e.image,...n.$attrs},{src:u(Xe)(typeof e.image=="string"?e.image:e.image.src),alt:e.alt??(typeof e.image=="string"?"":e.image.alt||"")}),null,16,ps)):(i(),l(F,{key:1},[E(a,ue({class:"dark",image:e.image.dark,alt:e.image.alt},n.$attrs),null,16,["image","alt"]),E(a,ue({class:"light",image:e.image.light,alt:e.image.alt},n.$attrs),null,16,["image","alt"])],64))],64)):g("",!0)}}}),Ve=q(vs,[["__scopeId","data-v-8426fc1a"]]),hs={class:"container"},fs={class:"main"},ms={class:"heading"},gs=["innerHTML"],xs=["innerHTML"],ks=["innerHTML"],bs={key:0,class:"actions"},ys={key:0,class:"image"},$s={class:"image-container"},Ss=w({__name:"VPHero",props:{name:{},text:{},tagline:{},image:{},actions:{}},setup(e){const n=Ne("hero-image-slot-exists");return(s,a)=>(i(),l("div",{class:N(["VPHero",{"has-image":e.image||u(n)}])},[t("div",hs,[t("div",fs,[x(s.$slots,"home-hero-info-before",{},void 0,!0),x(s.$slots,"home-hero-info",{},()=>[t("h1",ms,[e.name?(i(),l("span",{key:0,innerHTML:e.name,class:"name clip"},null,8,gs)):g("",!0),e.text?(i(),l("span",{key:1,innerHTML:e.text,class:"text"},null,8,xs)):g("",!0)]),e.tagline?(i(),l("p",{key:0,innerHTML:e.tagline,class:"tagline"},null,8,ks)):g("",!0)],!0),x(s.$slots,"home-hero-info-after",{},void 0,!0),e.actions?(i(),l("div",bs,[(i(!0),l(F,null,j(e.actions,o=>(i(),l("div",{key:o.link,class:"action"},[E(us,{tag:"a",size:"medium",theme:o.theme,text:o.text,href:o.link,target:o.target,rel:o.rel},null,8,["theme","text","href","target","rel"])]))),128))])):g("",!0),x(s.$slots,"home-hero-actions-after",{},void 0,!0)]),e.image||u(n)?(i(),l("div",ys,[t("div",$s,[a[0]||(a[0]=t("div",{class:"image-bg"},null,-1)),x(s.$slots,"home-hero-image",{},()=>[e.image?(i(),A(Ve,{key:0,class:"image-src",image:e.image},null,8,["image"])):g("",!0)],!0)])])):g("",!0)])],2))}}),Cs=q(Ss,[["__scopeId","data-v-4f9c455b"]]),Ls=w({__name:"VPHomeHero",setup(e){const{frontmatter:n}=U();return(s,a)=>u(n).hero?(i(),A(Cs,{key:0,class:"VPHomeHero",name:u(n).hero.name,text:u(n).hero.text,tagline:u(n).hero.tagline,image:u(n).hero.image,actions:u(n).hero.actions},{"home-hero-info-before":y(()=>[x(s.$slots,"home-hero-info-before")]),"home-hero-info":y(()=>[x(s.$slots,"home-hero-info")]),"home-hero-info-after":y(()=>[x(s.$slots,"home-hero-info-after")]),"home-hero-actions-after":y(()=>[x(s.$slots,"home-hero-actions-after")]),"home-hero-image":y(()=>[x(s.$slots,"home-hero-image")]),_:3},8,["name","text","tagline","image","actions"])):g("",!0)}}),Ps={class:"box"},Ts={key:0,class:"icon"},Os=["innerHTML"],Is=["innerHTML"],ws=["innerHTML"],Es={key:4,class:"link-text"},qs={class:"link-text-value"},Vs=w({__name:"VPFeature",props:{icon:{},title:{},details:{},link:{},linkText:{},rel:{},target:{}},setup(e){return(n,s)=>(i(),A(re,{class:"VPFeature",href:e.link,rel:e.rel,target:e.target,"no-icon":!0,tag:e.link?"a":"div"},{default:y(()=>[t("article",Ps,[typeof e.icon=="object"&&e.icon.wrap?(i(),l("div",Ts,[E(Ve,{image:e.icon,alt:e.icon.alt,height:e.icon.height||48,width:e.icon.width||48},null,8,["image","alt","height","width"])])):typeof e.icon=="object"?(i(),A(Ve,{key:1,image:e.icon,alt:e.icon.alt,height:e.icon.height||48,width:e.icon.width||48},null,8,["image","alt","height","width"])):e.icon?(i(),l("div",{key:2,class:"icon",innerHTML:e.icon},null,8,Os)):g("",!0),t("h2",{class:"title",innerHTML:e.title},null,8,Is),e.details?(i(),l("p",{key:3,class:"details",innerHTML:e.details},null,8,ws)):g("",!0),e.linkText?(i(),l("div",Es,[t("p",qs,[Q(v(e.linkText)+" ",1),s[0]||(s[0]=t("span",{class:"vpi-arrow-right link-text-icon"},null,-1))])])):g("",!0)])]),_:1},8,["href","rel","target","tag"]))}}),Ms=q(Vs,[["__scopeId","data-v-a3976bdc"]]),As={key:0,class:"VPFeatures"},Ds={class:"container"},Bs={class:"items"},Ns=w({__name:"VPFeatures",props:{features:{}},setup(e){const n=e,s=O(()=>{const a=n.features.length;if(a){if(a===2)return"grid-2";if(a===3)return"grid-3";if(a%3===0)return"grid-6";if(a>3)return"grid-4"}else return});return(a,o)=>e.features?(i(),l("div",As,[t("div",Ds,[t("div",Bs,[(i(!0),l(F,null,j(e.features,c=>(i(),l("div",{key:c.title,class:N(["item",[s.value]])},[E(Ms,{icon:c.icon,title:c.title,details:c.details,link:c.link,"link-text":c.linkText,rel:c.rel,target:c.target},null,8,["icon","title","details","link","link-text","rel","target"])],2))),128))])])])):g("",!0)}}),Fs=q(Ns,[["__scopeId","data-v-a6181336"]]),Rs=w({__name:"VPHomeFeatures",setup(e){const{frontmatter:n}=U();return(s,a)=>u(n).features?(i(),A(Fs,{key:0,class:"VPHomeFeatures",features:u(n).features},null,8,["features"])):g("",!0)}}),Hs=w({__name:"VPHomeContent",setup(e){const{width:n}=zt({initialWidth:0,includeScrollbar:!1});return(s,a)=>(i(),l("div",{class:"vp-doc container",style:W(u(n)?{"--vp-offset":`calc(50% - ${u(n)/2}px)`}:{})},[x(s.$slots,"default",{},void 0,!0)],4))}}),zs=q(Hs,[["__scopeId","data-v-8e2d4988"]]),Us=w({__name:"VPHome",setup(e){const{frontmatter:n,theme:s}=U();return(a,o)=>{const c=ke("Content");return i(),l("div",{class:N(["VPHome",{"external-link-icon-enabled":u(s).externalLinkIcon}])},[x(a.$slots,"home-hero-before",{},void 0,!0),E(Ls,null,{"home-hero-info-before":y(()=>[x(a.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":y(()=>[x(a.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":y(()=>[x(a.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":y(()=>[x(a.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":y(()=>[x(a.$slots,"home-hero-image",{},void 0,!0)]),_:3}),x(a.$slots,"home-hero-after",{},void 0,!0),x(a.$slots,"home-features-before",{},void 0,!0),E(Rs),x(a.$slots,"home-features-after",{},void 0,!0),u(n).markdownStyles!==!1?(i(),A(zs,{key:0},{default:y(()=>[E(c)]),_:1})):(i(),A(c,{key:1}))],2)}}}),Gs=q(Us,[["__scopeId","data-v-8b561e3d"]]),js={},Js={class:"VPPage"};function Qs(e,n){const s=ke("Content");return i(),l("div",Js,[x(e.$slots,"page-top"),E(s),x(e.$slots,"page-bottom")])}const Ws=q(js,[["render",Qs]]),Ks=w({__name:"VPContent",setup(e){const{page:n,frontmatter:s}=U(),{hasSidebar:a}=_e();return(o,c)=>(i(),l("div",{class:N(["VPContent",{"has-sidebar":u(a),"is-home":u(s).layout==="home"}]),id:"VPContent"},[u(n).isNotFound?x(o.$slots,"not-found",{key:0},()=>[E(vn)],!0):u(s).layout==="page"?(i(),A(Ws,{key:1},{"page-top":y(()=>[x(o.$slots,"page-top",{},void 0,!0)]),"page-bottom":y(()=>[x(o.$slots,"page-bottom",{},void 0,!0)]),_:3})):u(s).layout==="home"?(i(),A(Gs,{key:2},{"home-hero-before":y(()=>[x(o.$slots,"home-hero-before",{},void 0,!0)]),"home-hero-info-before":y(()=>[x(o.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":y(()=>[x(o.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":y(()=>[x(o.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":y(()=>[x(o.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":y(()=>[x(o.$slots,"home-hero-image",{},void 0,!0)]),"home-hero-after":y(()=>[x(o.$slots,"home-hero-after",{},void 0,!0)]),"home-features-before":y(()=>[x(o.$slots,"home-features-before",{},void 0,!0)]),"home-features-after":y(()=>[x(o.$slots,"home-features-after",{},void 0,!0)]),_:3})):u(s).layout&&u(s).layout!=="doc"?(i(),A(ce(u(s).layout),{key:3})):(i(),A(ds,{key:4},{"doc-top":y(()=>[x(o.$slots,"doc-top",{},void 0,!0)]),"doc-bottom":y(()=>[x(o.$slots,"doc-bottom",{},void 0,!0)]),"doc-footer-before":y(()=>[x(o.$slots,"doc-footer-before",{},void 0,!0)]),"doc-before":y(()=>[x(o.$slots,"doc-before",{},void 0,!0)]),"doc-after":y(()=>[x(o.$slots,"doc-after",{},void 0,!0)]),"aside-top":y(()=>[x(o.$slots,"aside-top",{},void 0,!0)]),"aside-outline-before":y(()=>[x(o.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":y(()=>[x(o.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":y(()=>[x(o.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":y(()=>[x(o.$slots,"aside-ads-after",{},void 0,!0)]),"aside-bottom":y(()=>[x(o.$slots,"aside-bottom",{},void 0,!0)]),_:3}))],2))}}),Ys=q(Ks,[["__scopeId","data-v-1428d186"]]),Xs={class:"container"},Zs=["innerHTML"],eo=["innerHTML"],to=w({__name:"VPFooter",setup(e){const{theme:n,frontmatter:s}=U(),{hasSidebar:a}=_e();return(o,c)=>u(n).footer&&u(s).footer!==!1?(i(),l("footer",{key:0,class:N(["VPFooter",{"has-sidebar":u(a)}])},[t("div",Xs,[u(n).footer.message?(i(),l("p",{key:0,class:"message",innerHTML:u(n).footer.message},null,8,Zs)):g("",!0),u(n).footer.copyright?(i(),l("p",{key:1,class:"copyright",innerHTML:u(n).footer.copyright},null,8,eo)):g("",!0)])],2)):g("",!0)}}),no=q(to,[["__scopeId","data-v-e315a0ad"]]);function so(){const{theme:e,frontmatter:n}=U(),s=ht([]),a=O(()=>s.value.length>0);return De(()=>{s.value=nt(n.value.outline??e.value.outline)}),{headers:s,hasLocalNav:a}}const oo={class:"menu-text"},io={class:"header"},ao={class:"outline"},lo=w({__name:"VPLocalNavOutlineDropdown",props:{headers:{},navHeight:{}},setup(e){const n=e,{theme:s}=U(),a=C(!1),o=C(0),c=C(),r=C();function d(h){var S;(S=c.value)!=null&&S.contains(h.target)||(a.value=!1)}ne(a,h=>{if(h){document.addEventListener("click",d);return}document.removeEventListener("click",d)}),Ut("Escape",()=>{a.value=!1}),De(()=>{a.value=!1});function _(){a.value=!a.value,o.value=window.innerHeight+Math.min(window.scrollY-n.navHeight,0)}function p(h){h.target.classList.contains("outline-link")&&(r.value&&(r.value.style.transition="none"),Pe(()=>{a.value=!1}))}function m(){a.value=!1,window.scrollTo({top:0,left:0,behavior:"smooth"})}return(h,S)=>(i(),l("div",{class:"VPLocalNavOutlineDropdown",style:W({"--vp-vh":o.value+"px"}),ref_key:"main",ref:c},[e.headers.length>0?(i(),l("button",{key:0,onClick:_,class:N({open:a.value})},[t("span",oo,v(u(yt)(u(s))),1),S[0]||(S[0]=t("span",{class:"vpi-chevron-right icon"},null,-1))],2)):(i(),l("button",{key:1,onClick:m},v(u(s).returnToTopLabel||"Return to top"),1)),E(se,{name:"flyout"},{default:y(()=>[a.value?(i(),l("div",{key:0,ref_key:"items",ref:r,class:"items",onClick:p},[t("div",io,[t("a",{class:"top-link",href:"#",onClick:m},v(u(s).returnToTopLabel||"Return to top"),1)]),t("div",ao,[E($t,{headers:e.headers},null,8,["headers"])])],512)):g("",!0)]),_:1})],4))}}),co=q(lo,[["__scopeId","data-v-8a42e2b4"]]),ro={class:"container"},_o=["aria-expanded"],uo={class:"menu-text"},po=w({__name:"VPLocalNav",props:{open:{type:Boolean}},emits:["open-menu"],setup(e){const{theme:n,frontmatter:s}=U(),{hasSidebar:a}=_e(),{headers:o}=so(),{y:c}=mt(),r=C(0);ee(()=>{r.value=parseInt(getComputedStyle(document.documentElement).getPropertyValue("--vp-nav-height"))}),De(()=>{o.value=nt(s.value.outline??n.value.outline)});const d=O(()=>o.value.length===0),_=O(()=>d.value&&!a.value),p=O(()=>({VPLocalNav:!0,"has-sidebar":a.value,empty:d.value,fixed:_.value}));return(m,h)=>u(s).layout!=="home"&&(!_.value||u(c)>=r.value)?(i(),l("div",{key:0,class:N(p.value)},[t("div",ro,[u(a)?(i(),l("button",{key:0,class:"menu","aria-expanded":e.open,"aria-controls":"VPSidebarNav",onClick:h[0]||(h[0]=S=>m.$emit("open-menu"))},[h[1]||(h[1]=t("span",{class:"vpi-align-left menu-icon"},null,-1)),t("span",uo,v(u(n).sidebarMenuLabel||"Menu"),1)],8,_o)):g("",!0),E(co,{headers:u(o),navHeight:r.value},null,8,["headers","navHeight"])])],2)):g("",!0)}}),vo=q(po,[["__scopeId","data-v-a6f0e41e"]]);function ho(){const e=C(!1);function n(){e.value=!0,window.addEventListener("resize",o)}function s(){e.value=!1,window.removeEventListener("resize",o)}function a(){e.value?s():n()}function o(){window.outerWidth>=768&&s()}const c=Be();return ne(()=>c.path,s),{isScreenOpen:e,openScreen:n,closeScreen:s,toggleScreen:a}}const fo={},mo={class:"VPSwitch",type:"button",role:"switch"},go={class:"check"},xo={key:0,class:"icon"};function ko(e,n){return i(),l("button",mo,[t("span",go,[e.$slots.default?(i(),l("span",xo,[x(e.$slots,"default",{},void 0,!0)])):g("",!0)])])}const bo=q(fo,[["render",ko],["__scopeId","data-v-1d5665e3"]]),yo=w({__name:"VPSwitchAppearance",setup(e){const{isDark:n,theme:s}=U(),a=Ne("toggle-appearance",()=>{n.value=!n.value}),o=C("");return Ze(()=>{o.value=n.value?s.value.lightModeSwitchTitle||"Switch to light theme":s.value.darkModeSwitchTitle||"Switch to dark theme"}),(c,r)=>(i(),A(bo,{title:o.value,class:"VPSwitchAppearance","aria-checked":u(n),onClick:u(a)},{default:y(()=>[...r[0]||(r[0]=[t("span",{class:"vpi-sun sun"},null,-1),t("span",{class:"vpi-moon moon"},null,-1)])]),_:1},8,["title","aria-checked","onClick"]))}}),st=q(yo,[["__scopeId","data-v-5337faa4"]]),$o={key:0,class:"VPNavBarAppearance"},So=w({__name:"VPNavBarAppearance",setup(e){const{site:n}=U();return(s,a)=>u(n).appearance&&u(n).appearance!=="force-dark"&&u(n).appearance!=="force-auto"?(i(),l("div",$o,[E(st)])):g("",!0)}}),Co=q(So,[["__scopeId","data-v-6c893767"]]),ot=C();let St=!1,Ge=0;function Lo(e){const n=C(!1);if(Fe){!St&&Po(),Ge++;const s=ne(ot,a=>{var o,c,r;a===e.el.value||(o=e.el.value)!=null&&o.contains(a)?(n.value=!0,(c=e.onFocus)==null||c.call(e)):(n.value=!1,(r=e.onBlur)==null||r.call(e))});de(()=>{s(),Ge--,Ge||To()})}return Ee(n)}function Po(){document.addEventListener("focusin",Ct),St=!0,ot.value=document.activeElement}function To(){document.removeEventListener("focusin",Ct)}function Ct(){ot.value=document.activeElement}const Oo={class:"VPMenuLink"},Io=["innerHTML"],wo=w({__name:"VPMenuLink",props:{item:{}},setup(e){const{page:n}=U();return(s,a)=>(i(),l("div",Oo,[E(re,{class:N({active:u(xe)(u(n).relativePath,e.item.activeMatch||e.item.link,!!e.item.activeMatch)}),href:e.item.link,target:e.item.target,rel:e.item.rel,"no-icon":e.item.noIcon},{default:y(()=>[t("span",{innerHTML:e.item.text},null,8,Io)]),_:1},8,["class","href","target","rel","no-icon"])]))}}),He=q(wo,[["__scopeId","data-v-35975db6"]]),Eo={class:"VPMenuGroup"},qo={key:0,class:"title"},Vo=w({__name:"VPMenuGroup",props:{text:{},items:{}},setup(e){return(n,s)=>(i(),l("div",Eo,[e.text?(i(),l("p",qo,v(e.text),1)):g("",!0),(i(!0),l(F,null,j(e.items,a=>(i(),l(F,null,["link"in a?(i(),A(He,{key:0,item:a},null,8,["item"])):g("",!0)],64))),256))]))}}),Mo=q(Vo,[["__scopeId","data-v-69e747b5"]]),Ao={class:"VPMenu"},Do={key:0,class:"items"},Bo=w({__name:"VPMenu",props:{items:{}},setup(e){return(n,s)=>(i(),l("div",Ao,[e.items?(i(),l("div",Do,[(i(!0),l(F,null,j(e.items,a=>(i(),l(F,{key:JSON.stringify(a)},["link"in a?(i(),A(He,{key:0,item:a},null,8,["item"])):"component"in a?(i(),A(ce(a.component),ue({key:1,ref_for:!0},a.props),null,16)):(i(),A(Mo,{key:2,text:a.text,items:a.items},null,8,["text","items"]))],64))),128))])):g("",!0),x(n.$slots,"default",{},void 0,!0)]))}}),No=q(Bo,[["__scopeId","data-v-b98bc113"]]),Fo=["aria-expanded","aria-label"],Ro={key:0,class:"text"},Ho=["innerHTML"],zo={key:1,class:"vpi-more-horizontal icon"},Uo={class:"menu"},Go=w({__name:"VPFlyout",props:{icon:{},button:{},label:{},items:{}},setup(e){const n=C(!1),s=C();Lo({el:s,onBlur:a});function a(){n.value=!1}return(o,c)=>(i(),l("div",{class:"VPFlyout",ref_key:"el",ref:s,onMouseenter:c[1]||(c[1]=r=>n.value=!0),onMouseleave:c[2]||(c[2]=r=>n.value=!1)},[t("button",{type:"button",class:"button","aria-haspopup":"true","aria-expanded":n.value,"aria-label":e.label,onClick:c[0]||(c[0]=r=>n.value=!n.value)},[e.button||e.icon?(i(),l("span",Ro,[e.icon?(i(),l("span",{key:0,class:N([e.icon,"option-icon"])},null,2)):g("",!0),e.button?(i(),l("span",{key:1,innerHTML:e.button},null,8,Ho)):g("",!0),c[3]||(c[3]=t("span",{class:"vpi-chevron-down text-icon"},null,-1))])):(i(),l("span",zo))],8,Fo),t("div",Uo,[E(No,{items:e.items},{default:y(()=>[x(o.$slots,"default",{},void 0,!0)]),_:3},8,["items"])])],544))}}),it=q(Go,[["__scopeId","data-v-cf11d7a2"]]),jo=["href","aria-label","innerHTML"],Jo=w({__name:"VPSocialLink",props:{icon:{},link:{},ariaLabel:{}},setup(e){const n=e,s=C();ee(async()=>{var c;await Pe();const o=(c=s.value)==null?void 0:c.children[0];o instanceof HTMLElement&&o.className.startsWith("vpi-social-")&&(getComputedStyle(o).maskImage||getComputedStyle(o).webkitMaskImage)==="none"&&o.style.setProperty("--icon",`url('https://api.iconify.design/simple-icons/${n.icon}.svg')`)});const a=O(()=>typeof n.icon=="object"?n.icon.svg:`<span class="vpi-social-${n.icon}"></span>`);return(o,c)=>(i(),l("a",{ref_key:"el",ref:s,class:"VPSocialLink no-icon",href:e.link,"aria-label":e.ariaLabel??(typeof e.icon=="string"?e.icon:""),target:"_blank",rel:"noopener",innerHTML:a.value},null,8,jo))}}),Qo=q(Jo,[["__scopeId","data-v-bd121fe5"]]),Wo={class:"VPSocialLinks"},Ko=w({__name:"VPSocialLinks",props:{links:{}},setup(e){return(n,s)=>(i(),l("div",Wo,[(i(!0),l(F,null,j(e.links,({link:a,icon:o,ariaLabel:c})=>(i(),A(Qo,{key:a,icon:o,link:a,ariaLabel:c},null,8,["icon","link","ariaLabel"]))),128))]))}}),at=q(Ko,[["__scopeId","data-v-7bc22406"]]),Yo={key:0,class:"group translations"},Xo={class:"trans-title"},Zo={key:1,class:"group"},ei={class:"item appearance"},ti={class:"label"},ni={class:"appearance-action"},si={key:2,class:"group"},oi={class:"item social-links"},ii=w({__name:"VPNavBarExtra",setup(e){const{site:n,theme:s}=U(),{localeLinks:a,currentLang:o}=Oe({correspondingLink:!0}),c=O(()=>a.value.length&&o.value.label||n.value.appearance||s.value.socialLinks);return(r,d)=>c.value?(i(),A(it,{key:0,class:"VPNavBarExtra",label:"extra navigation"},{default:y(()=>[u(a).length&&u(o).label?(i(),l("div",Yo,[t("p",Xo,v(u(o).label),1),(i(!0),l(F,null,j(u(a),_=>(i(),A(He,{key:_.link,item:_},null,8,["item"]))),128))])):g("",!0),u(n).appearance&&u(n).appearance!=="force-dark"&&u(n).appearance!=="force-auto"?(i(),l("div",Zo,[t("div",ei,[t("p",ti,v(u(s).darkModeSwitchLabel||"Appearance"),1),t("div",ni,[E(st)])])])):g("",!0),u(s).socialLinks?(i(),l("div",si,[t("div",oi,[E(at,{class:"social-links-list",links:u(s).socialLinks},null,8,["links"])])])):g("",!0)]),_:1})):g("",!0)}}),ai=q(ii,[["__scopeId","data-v-bb2aa2f0"]]),li=["aria-expanded"],ci=w({__name:"VPNavBarHamburger",props:{active:{type:Boolean}},emits:["click"],setup(e){return(n,s)=>(i(),l("button",{type:"button",class:N(["VPNavBarHamburger",{active:e.active}]),"aria-label":"mobile navigation","aria-expanded":e.active,"aria-controls":"VPNavScreen",onClick:s[0]||(s[0]=a=>n.$emit("click"))},[...s[1]||(s[1]=[t("span",{class:"container"},[t("span",{class:"top"}),t("span",{class:"middle"}),t("span",{class:"bottom"})],-1)])],10,li))}}),ri=q(ci,[["__scopeId","data-v-e5dd9c1c"]]),di=["innerHTML"],_i=w({__name:"VPNavBarMenuLink",props:{item:{}},setup(e){const{page:n}=U();return(s,a)=>(i(),A(re,{class:N({VPNavBarMenuLink:!0,active:u(xe)(u(n).relativePath,e.item.activeMatch||e.item.link,!!e.item.activeMatch)}),href:e.item.link,target:e.item.target,rel:e.item.rel,"no-icon":e.item.noIcon,tabindex:"0"},{default:y(()=>[t("span",{innerHTML:e.item.text},null,8,di)]),_:1},8,["class","href","target","rel","no-icon"]))}}),ui=q(_i,[["__scopeId","data-v-e56f3d57"]]),pi=w({__name:"VPNavBarMenuGroup",props:{item:{}},setup(e){const n=e,{page:s}=U(),a=c=>"component"in c?!1:"link"in c?xe(s.value.relativePath,c.link,!!n.item.activeMatch):c.items.some(a),o=O(()=>a(n.item));return(c,r)=>(i(),A(it,{class:N({VPNavBarMenuGroup:!0,active:u(xe)(u(s).relativePath,e.item.activeMatch,!!e.item.activeMatch)||o.value}),button:e.item.text,items:e.item.items},null,8,["class","button","items"]))}}),vi={key:0,"aria-labelledby":"main-nav-aria-label",class:"VPNavBarMenu"},hi=w({__name:"VPNavBarMenu",setup(e){const{theme:n}=U();return(s,a)=>u(n).nav?(i(),l("nav",vi,[a[0]||(a[0]=t("span",{id:"main-nav-aria-label",class:"visually-hidden"}," Main Navigation ",-1)),(i(!0),l(F,null,j(u(n).nav,o=>(i(),l(F,{key:JSON.stringify(o)},["link"in o?(i(),A(ui,{key:0,item:o},null,8,["item"])):"component"in o?(i(),A(ce(o.component),ue({key:1,ref_for:!0},o.props),null,16)):(i(),A(pi,{key:2,item:o},null,8,["item"]))],64))),128))])):g("",!0)}}),fi=q(hi,[["__scopeId","data-v-dc692963"]]);function mi(e){const{localeIndex:n,theme:s}=U();function a(o){var b,I,M;const c=o.split("."),r=(b=s.value.search)==null?void 0:b.options,d=r&&typeof r=="object",_=d&&((M=(I=r.locales)==null?void 0:I[n.value])==null?void 0:M.translations)||null,p=d&&r.translations||null;let m=_,h=p,S=e;const f=c.pop();for(const H of c){let V=null;const P=S==null?void 0:S[H];P&&(V=S=P);const L=h==null?void 0:h[H];L&&(V=h=L);const T=m==null?void 0:m[H];T&&(V=m=T),P||(S=V),L||(h=V),T||(m=V)}return(m==null?void 0:m[f])??(h==null?void 0:h[f])??(S==null?void 0:S[f])??""}return a}const gi=["aria-label"],xi={class:"DocSearch-Button-Container"},ki={class:"DocSearch-Button-Placeholder"},ct=w({__name:"VPNavBarSearchButton",setup(e){const s=mi({button:{buttonText:"Search",buttonAriaLabel:"Search"}});return(a,o)=>(i(),l("button",{type:"button",class:"DocSearch DocSearch-Button","aria-label":u(s)("button.buttonAriaLabel")},[t("span",xi,[o[0]||(o[0]=t("span",{class:"vp-icon DocSearch-Search-Icon"},null,-1)),t("span",ki,v(u(s)("button.buttonText")),1)]),o[1]||(o[1]=t("span",{class:"DocSearch-Button-Keys"},[t("kbd",{class:"DocSearch-Button-Key"}),t("kbd",{class:"DocSearch-Button-Key"},"K")],-1))],8,gi))}}),bi={class:"VPNavBarSearch"},yi={id:"local-search"},$i={key:1,id:"docsearch"},Si=w({__name:"VPNavBarSearch",setup(e){const n=()=>null,s=Gt(()=>Z(()=>import("./VPAlgoliaSearchBox.CPS0RSuv.js"),__vite__mapDeps([0,1]))),{theme:a}=U(),o=C(!1),c=C(!1),r=()=>{const S="VPAlgoliaPreconnect";(window.requestIdleCallback||setTimeout)(()=>{var I;const b=document.createElement("link");b.id=S,b.rel="preconnect",b.href=`https://${(((I=a.value.search)==null?void 0:I.options)??a.value.algolia).appId}-dsn.algolia.net`,b.crossOrigin="",document.head.appendChild(b)})};ee(()=>{r();const S=b=>{(b.key.toLowerCase()==="k"&&(b.metaKey||b.ctrlKey)||!p(b)&&b.key==="/")&&(b.preventDefault(),d(),f())},f=()=>{window.removeEventListener("keydown",S)};window.addEventListener("keydown",S),de(f)});function d(){o.value||(o.value=!0,setTimeout(_,16))}function _(){const S=new Event("keydown");S.key="k",S.metaKey=!0,window.dispatchEvent(S),setTimeout(()=>{document.querySelector(".DocSearch-Modal")||_()},16)}function p(S){const f=S.target,b=f.tagName;return f.isContentEditable||b==="INPUT"||b==="SELECT"||b==="TEXTAREA"}const m=C(!1),h="algolia";return(S,f)=>{var b;return i(),l("div",bi,[u(h)==="local"?(i(),l(F,{key:0},[m.value?(i(),A(u(n),{key:0,onClose:f[0]||(f[0]=I=>m.value=!1)})):g("",!0),t("div",yi,[E(ct,{onClick:f[1]||(f[1]=I=>m.value=!0)})])],64)):u(h)==="algolia"?(i(),l(F,{key:1},[o.value?(i(),A(u(s),{key:0,algolia:((b=u(a).search)==null?void 0:b.options)??u(a).algolia,onVnodeBeforeMount:f[2]||(f[2]=I=>c.value=!0)},null,8,["algolia"])):g("",!0),c.value?g("",!0):(i(),l("div",$i,[E(ct,{onClick:d})]))],64)):g("",!0)])}}}),Ci=w({__name:"VPNavBarSocialLinks",setup(e){const{theme:n}=U();return(s,a)=>u(n).socialLinks?(i(),A(at,{key:0,class:"VPNavBarSocialLinks",links:u(n).socialLinks},null,8,["links"])):g("",!0)}}),Li=q(Ci,[["__scopeId","data-v-0394ad82"]]),Pi=["href","rel","target"],Ti=["innerHTML"],Oi={key:2},Ii=w({__name:"VPNavBarTitle",setup(e){const{site:n,theme:s}=U(),{hasSidebar:a}=_e(),{currentLang:o}=Oe(),c=O(()=>{var _;return typeof s.value.logoLink=="string"?s.value.logoLink:(_=s.value.logoLink)==null?void 0:_.link}),r=O(()=>{var _;return typeof s.value.logoLink=="string"||(_=s.value.logoLink)==null?void 0:_.rel}),d=O(()=>{var _;return typeof s.value.logoLink=="string"||(_=s.value.logoLink)==null?void 0:_.target});return(_,p)=>(i(),l("div",{class:N(["VPNavBarTitle",{"has-sidebar":u(a)}])},[t("a",{class:"title",href:c.value??u(tt)(u(o).link),rel:r.value,target:d.value},[x(_.$slots,"nav-bar-title-before",{},void 0,!0),u(s).logo?(i(),A(Ve,{key:0,class:"logo",image:u(s).logo},null,8,["image"])):g("",!0),u(s).siteTitle?(i(),l("span",{key:1,innerHTML:u(s).siteTitle},null,8,Ti)):u(s).siteTitle===void 0?(i(),l("span",Oi,v(u(n).title),1)):g("",!0),x(_.$slots,"nav-bar-title-after",{},void 0,!0)],8,Pi)],2))}}),wi=q(Ii,[["__scopeId","data-v-1168a8e4"]]),Ei={class:"items"},qi={class:"title"},Vi=w({__name:"VPNavBarTranslations",setup(e){const{theme:n}=U(),{localeLinks:s,currentLang:a}=Oe({correspondingLink:!0});return(o,c)=>u(s).length&&u(a).label?(i(),A(it,{key:0,class:"VPNavBarTranslations",icon:"vpi-languages",label:u(n).langMenuLabel||"Change language"},{default:y(()=>[t("div",Ei,[t("p",qi,v(u(a).label),1),(i(!0),l(F,null,j(u(s),r=>(i(),A(He,{key:r.link,item:r},null,8,["item"]))),128))])]),_:1},8,["label"])):g("",!0)}}),Mi=q(Vi,[["__scopeId","data-v-88af2de4"]]),Ai={class:"wrapper"},Di={class:"container"},Bi={class:"title"},Ni={class:"content"},Fi={class:"content-body"},Ri=w({__name:"VPNavBar",props:{isScreenOpen:{type:Boolean}},emits:["toggle-screen"],setup(e){const n=e,{y:s}=mt(),{hasSidebar:a}=_e(),{frontmatter:o}=U(),c=C({});return Ze(()=>{c.value={"has-sidebar":a.value,home:o.value.layout==="home",top:s.value===0,"screen-open":n.isScreenOpen}}),(r,d)=>(i(),l("div",{class:N(["VPNavBar",c.value])},[t("div",Ai,[t("div",Di,[t("div",Bi,[E(wi,null,{"nav-bar-title-before":y(()=>[x(r.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":y(()=>[x(r.$slots,"nav-bar-title-after",{},void 0,!0)]),_:3})]),t("div",Ni,[t("div",Fi,[x(r.$slots,"nav-bar-content-before",{},void 0,!0),E(Si,{class:"search"}),E(fi,{class:"menu"}),E(Mi,{class:"translations"}),E(Co,{class:"appearance"}),E(Li,{class:"social-links"}),E(ai,{class:"extra"}),x(r.$slots,"nav-bar-content-after",{},void 0,!0),E(ri,{class:"hamburger",active:e.isScreenOpen,onClick:d[0]||(d[0]=_=>r.$emit("toggle-screen"))},null,8,["active"])])])])]),d[1]||(d[1]=t("div",{class:"divider"},[t("div",{class:"divider-line"})],-1))],2))}}),Hi=q(Ri,[["__scopeId","data-v-6aa21345"]]),zi={key:0,class:"VPNavScreenAppearance"},Ui={class:"text"},Gi=w({__name:"VPNavScreenAppearance",setup(e){const{site:n,theme:s}=U();return(a,o)=>u(n).appearance&&u(n).appearance!=="force-dark"&&u(n).appearance!=="force-auto"?(i(),l("div",zi,[t("p",Ui,v(u(s).darkModeSwitchLabel||"Appearance"),1),E(st)])):g("",!0)}}),ji=q(Gi,[["__scopeId","data-v-b44890b2"]]),Ji=["innerHTML"],Qi=w({__name:"VPNavScreenMenuLink",props:{item:{}},setup(e){const n=Ne("close-screen");return(s,a)=>(i(),A(re,{class:"VPNavScreenMenuLink",href:e.item.link,target:e.item.target,rel:e.item.rel,"no-icon":e.item.noIcon,onClick:u(n)},{default:y(()=>[t("span",{innerHTML:e.item.text},null,8,Ji)]),_:1},8,["href","target","rel","no-icon","onClick"]))}}),Wi=q(Qi,[["__scopeId","data-v-df37e6dd"]]),Ki=["innerHTML"],Yi=w({__name:"VPNavScreenMenuGroupLink",props:{item:{}},setup(e){const n=Ne("close-screen");return(s,a)=>(i(),A(re,{class:"VPNavScreenMenuGroupLink",href:e.item.link,target:e.item.target,rel:e.item.rel,"no-icon":e.item.noIcon,onClick:u(n)},{default:y(()=>[t("span",{innerHTML:e.item.text},null,8,Ki)]),_:1},8,["href","target","rel","no-icon","onClick"]))}}),Lt=q(Yi,[["__scopeId","data-v-3e9c20e4"]]),Xi={class:"VPNavScreenMenuGroupSection"},Zi={key:0,class:"title"},ea=w({__name:"VPNavScreenMenuGroupSection",props:{text:{},items:{}},setup(e){return(n,s)=>(i(),l("div",Xi,[e.text?(i(),l("p",Zi,v(e.text),1)):g("",!0),(i(!0),l(F,null,j(e.items,a=>(i(),A(Lt,{key:a.text,item:a},null,8,["item"]))),128))]))}}),ta=q(ea,[["__scopeId","data-v-8133b170"]]),na=["aria-controls","aria-expanded"],sa=["innerHTML"],oa=["id"],ia={key:0,class:"item"},aa={key:1,class:"item"},la={key:2,class:"group"},ca=w({__name:"VPNavScreenMenuGroup",props:{text:{},items:{}},setup(e){const n=e,s=C(!1),a=O(()=>`NavScreenGroup-${n.text.replace(" ","-").toLowerCase()}`);function o(){s.value=!s.value}return(c,r)=>(i(),l("div",{class:N(["VPNavScreenMenuGroup",{open:s.value}])},[t("button",{class:"button","aria-controls":a.value,"aria-expanded":s.value,onClick:o},[t("span",{class:"button-text",innerHTML:e.text},null,8,sa),r[0]||(r[0]=t("span",{class:"vpi-plus button-icon"},null,-1))],8,na),t("div",{id:a.value,class:"items"},[(i(!0),l(F,null,j(e.items,d=>(i(),l(F,{key:JSON.stringify(d)},["link"in d?(i(),l("div",ia,[E(Lt,{item:d},null,8,["item"])])):"component"in d?(i(),l("div",aa,[(i(),A(ce(d.component),ue({ref_for:!0},d.props,{"screen-menu":""}),null,16))])):(i(),l("div",la,[E(ta,{text:d.text,items:d.items},null,8,["text","items"])]))],64))),128))],8,oa)],2))}}),ra=q(ca,[["__scopeId","data-v-b9ab8c58"]]),da={key:0,class:"VPNavScreenMenu"},_a=w({__name:"VPNavScreenMenu",setup(e){const{theme:n}=U();return(s,a)=>u(n).nav?(i(),l("nav",da,[(i(!0),l(F,null,j(u(n).nav,o=>(i(),l(F,{key:JSON.stringify(o)},["link"in o?(i(),A(Wi,{key:0,item:o},null,8,["item"])):"component"in o?(i(),A(ce(o.component),ue({key:1,ref_for:!0},o.props,{"screen-menu":""}),null,16)):(i(),A(ra,{key:2,text:o.text||"",items:o.items},null,8,["text","items"]))],64))),128))])):g("",!0)}}),ua=w({__name:"VPNavScreenSocialLinks",setup(e){const{theme:n}=U();return(s,a)=>u(n).socialLinks?(i(),A(at,{key:0,class:"VPNavScreenSocialLinks",links:u(n).socialLinks},null,8,["links"])):g("",!0)}}),pa={class:"list"},va=w({__name:"VPNavScreenTranslations",setup(e){const{localeLinks:n,currentLang:s}=Oe({correspondingLink:!0}),a=C(!1);function o(){a.value=!a.value}return(c,r)=>u(n).length&&u(s).label?(i(),l("div",{key:0,class:N(["VPNavScreenTranslations",{open:a.value}])},[t("button",{class:"title",onClick:o},[r[0]||(r[0]=t("span",{class:"vpi-languages icon lang"},null,-1)),Q(" "+v(u(s).label)+" ",1),r[1]||(r[1]=t("span",{class:"vpi-chevron-down icon chevron"},null,-1))]),t("ul",pa,[(i(!0),l(F,null,j(u(n),d=>(i(),l("li",{key:d.link,class:"item"},[E(re,{class:"link",href:d.link},{default:y(()=>[Q(v(d.text),1)]),_:2},1032,["href"])]))),128))])],2)):g("",!0)}}),ha=q(va,[["__scopeId","data-v-858fe1a4"]]),fa={class:"container"},ma=w({__name:"VPNavScreen",props:{open:{type:Boolean}},setup(e){const n=C(null),s=gt(Fe?document.body:null);return(a,o)=>(i(),A(se,{name:"fade",onEnter:o[0]||(o[0]=c=>s.value=!0),onAfterLeave:o[1]||(o[1]=c=>s.value=!1)},{default:y(()=>[e.open?(i(),l("div",{key:0,class:"VPNavScreen",ref_key:"screen",ref:n,id:"VPNavScreen"},[t("div",fa,[x(a.$slots,"nav-screen-content-before",{},void 0,!0),E(_a,{class:"menu"}),E(ha,{class:"translations"}),E(ji,{class:"appearance"}),E(ua,{class:"social-links"}),x(a.$slots,"nav-screen-content-after",{},void 0,!0)])],512)):g("",!0)]),_:3}))}}),ga=q(ma,[["__scopeId","data-v-f2779853"]]),xa={key:0,class:"VPNav"},ka=w({__name:"VPNav",setup(e){const{isScreenOpen:n,closeScreen:s,toggleScreen:a}=ho(),{frontmatter:o}=U(),c=O(()=>o.value.navbar!==!1);return xt("close-screen",s),Ae(()=>{Fe&&document.documentElement.classList.toggle("hide-nav",!c.value)}),(r,d)=>c.value?(i(),l("header",xa,[E(Hi,{"is-screen-open":u(n),onToggleScreen:u(a)},{"nav-bar-title-before":y(()=>[x(r.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":y(()=>[x(r.$slots,"nav-bar-title-after",{},void 0,!0)]),"nav-bar-content-before":y(()=>[x(r.$slots,"nav-bar-content-before",{},void 0,!0)]),"nav-bar-content-after":y(()=>[x(r.$slots,"nav-bar-content-after",{},void 0,!0)]),_:3},8,["is-screen-open","onToggleScreen"]),E(ga,{open:u(n)},{"nav-screen-content-before":y(()=>[x(r.$slots,"nav-screen-content-before",{},void 0,!0)]),"nav-screen-content-after":y(()=>[x(r.$slots,"nav-screen-content-after",{},void 0,!0)]),_:3},8,["open"])])):g("",!0)}}),ba=q(ka,[["__scopeId","data-v-ae24b3ad"]]),ya=["role","tabindex"],$a={key:1,class:"items"},Sa=w({__name:"VPSidebarItem",props:{item:{},depth:{}},setup(e){const n=e,{collapsed:s,collapsible:a,isLink:o,isActiveLink:c,hasActiveLink:r,hasChildren:d,toggle:_}=gn(O(()=>n.item)),p=O(()=>d.value?"section":"div"),m=O(()=>o.value?"a":"div"),h=O(()=>d.value?n.depth+2===7?"p":`h${n.depth+2}`:"p"),S=O(()=>o.value?void 0:"button"),f=O(()=>[[`level-${n.depth}`],{collapsible:a.value},{collapsed:s.value},{"is-link":o.value},{"is-active":c.value},{"has-active":r.value}]);function b(M){"key"in M&&M.key!=="Enter"||!n.item.link&&_()}function I(){n.item.link&&_()}return(M,H)=>{const V=ke("VPSidebarItem",!0);return i(),A(ce(p.value),{class:N(["VPSidebarItem",f.value])},{default:y(()=>[e.item.text?(i(),l("div",ue({key:0,class:"item",role:S.value},jt(e.item.items?{click:b,keydown:b}:{},!0),{tabindex:e.item.items&&0}),[H[1]||(H[1]=t("div",{class:"indicator"},null,-1)),e.item.link?(i(),A(re,{key:0,tag:m.value,class:"link",href:e.item.link,rel:e.item.rel,target:e.item.target},{default:y(()=>[(i(),A(ce(h.value),{class:"text",innerHTML:e.item.text},null,8,["innerHTML"]))]),_:1},8,["tag","href","rel","target"])):(i(),A(ce(h.value),{key:1,class:"text",innerHTML:e.item.text},null,8,["innerHTML"])),e.item.collapsed!=null&&e.item.items&&e.item.items.length?(i(),l("div",{key:2,class:"caret",role:"button","aria-label":"toggle section",onClick:I,onKeydown:Jt(I,["enter"]),tabindex:"0"},[...H[0]||(H[0]=[t("span",{class:"vpi-chevron-right caret-icon"},null,-1)])],32)):g("",!0)],16,ya)):g("",!0),e.item.items&&e.item.items.length?(i(),l("div",$a,[e.depth<5?(i(!0),l(F,{key:0},j(e.item.items,P=>(i(),A(V,{key:P.text,item:P,depth:e.depth+1},null,8,["item","depth"]))),128)):g("",!0)])):g("",!0)]),_:1},8,["class"])}}}),Ca=q(Sa,[["__scopeId","data-v-b3fd67f8"]]),La=w({__name:"VPSidebarGroup",props:{items:{}},setup(e){const n=C(!0);let s=null;return ee(()=>{s=setTimeout(()=>{s=null,n.value=!1},300)}),Qt(()=>{s!=null&&(clearTimeout(s),s=null)}),(a,o)=>(i(!0),l(F,null,j(e.items,c=>(i(),l("div",{key:c.text,class:N(["group",{"no-transition":n.value}])},[E(Ca,{item:c,depth:0},null,8,["item"])],2))),128))}}),Pa=q(La,[["__scopeId","data-v-c40bc020"]]),Ta={class:"nav",id:"VPSidebarNav","aria-labelledby":"sidebar-aria-label",tabindex:"-1"},Oa=w({__name:"VPSidebar",props:{open:{type:Boolean}},setup(e){const{sidebarGroups:n,hasSidebar:s}=_e(),a=e,o=C(null),c=gt(Fe?document.body:null);ne([a,o],()=>{var d;a.open?(c.value=!0,(d=o.value)==null||d.focus()):c.value=!1},{immediate:!0,flush:"post"});const r=C(0);return ne(n,()=>{r.value+=1},{deep:!0}),(d,_)=>u(s)?(i(),l("aside",{key:0,class:N(["VPSidebar",{open:e.open}]),ref_key:"navEl",ref:o,onClick:_[0]||(_[0]=Te(()=>{},["stop"]))},[_[2]||(_[2]=t("div",{class:"curtain"},null,-1)),t("nav",Ta,[_[1]||(_[1]=t("span",{class:"visually-hidden",id:"sidebar-aria-label"}," Sidebar Navigation ",-1)),x(d.$slots,"sidebar-nav-before",{},void 0,!0),(i(),A(Pa,{items:u(n),key:r.value},null,8,["items"])),x(d.$slots,"sidebar-nav-after",{},void 0,!0)])],2)):g("",!0)}}),Ia=q(Oa,[["__scopeId","data-v-319d5ca6"]]),wa=w({__name:"VPSkipLink",setup(e){const{theme:n}=U(),s=Be(),a=C();ne(()=>s.path,()=>a.value.focus());function o({target:c}){const r=document.getElementById(decodeURIComponent(c.hash).slice(1));if(r){const d=()=>{r.removeAttribute("tabindex"),r.removeEventListener("blur",d)};r.setAttribute("tabindex","-1"),r.addEventListener("blur",d),r.focus(),window.scrollTo(0,0)}}return(c,r)=>(i(),l(F,null,[t("span",{ref_key:"backToTop",ref:a,tabindex:"-1"},null,512),t("a",{href:"#VPContent",class:"VPSkipLink visually-hidden",onClick:o},v(u(n).skipToContentLabel||"Skip to content"),1)],64))}}),Ea=q(wa,[["__scopeId","data-v-0b0ada53"]]),qa=w({__name:"Layout",setup(e){const{isOpen:n,open:s,close:a}=_e(),o=Be();ne(()=>o.path,a),mn(n,a);const{frontmatter:c}=U(),r=Wt(),d=O(()=>!!r["home-hero-image"]);return xt("hero-image-slot-exists",d),(_,p)=>{const m=ke("Content");return u(c).layout!==!1?(i(),l("div",{key:0,class:N(["Layout",u(c).pageClass])},[x(_.$slots,"layout-top",{},void 0,!0),E(Ea),E(sn,{class:"backdrop",show:u(n),onClick:u(a)},null,8,["show","onClick"]),E(ba,null,{"nav-bar-title-before":y(()=>[x(_.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":y(()=>[x(_.$slots,"nav-bar-title-after",{},void 0,!0)]),"nav-bar-content-before":y(()=>[x(_.$slots,"nav-bar-content-before",{},void 0,!0)]),"nav-bar-content-after":y(()=>[x(_.$slots,"nav-bar-content-after",{},void 0,!0)]),"nav-screen-content-before":y(()=>[x(_.$slots,"nav-screen-content-before",{},void 0,!0)]),"nav-screen-content-after":y(()=>[x(_.$slots,"nav-screen-content-after",{},void 0,!0)]),_:3}),E(vo,{open:u(n),onOpenMenu:u(s)},null,8,["open","onOpenMenu"]),E(Ia,{open:u(n)},{"sidebar-nav-before":y(()=>[x(_.$slots,"sidebar-nav-before",{},void 0,!0)]),"sidebar-nav-after":y(()=>[x(_.$slots,"sidebar-nav-after",{},void 0,!0)]),_:3},8,["open"]),E(Ys,null,{"page-top":y(()=>[x(_.$slots,"page-top",{},void 0,!0)]),"page-bottom":y(()=>[x(_.$slots,"page-bottom",{},void 0,!0)]),"not-found":y(()=>[x(_.$slots,"not-found",{},void 0,!0)]),"home-hero-before":y(()=>[x(_.$slots,"home-hero-before",{},void 0,!0)]),"home-hero-info-before":y(()=>[x(_.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":y(()=>[x(_.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":y(()=>[x(_.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":y(()=>[x(_.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":y(()=>[x(_.$slots,"home-hero-image",{},void 0,!0)]),"home-hero-after":y(()=>[x(_.$slots,"home-hero-after",{},void 0,!0)]),"home-features-before":y(()=>[x(_.$slots,"home-features-before",{},void 0,!0)]),"home-features-after":y(()=>[x(_.$slots,"home-features-after",{},void 0,!0)]),"doc-footer-before":y(()=>[x(_.$slots,"doc-footer-before",{},void 0,!0)]),"doc-before":y(()=>[x(_.$slots,"doc-before",{},void 0,!0)]),"doc-after":y(()=>[x(_.$slots,"doc-after",{},void 0,!0)]),"doc-top":y(()=>[x(_.$slots,"doc-top",{},void 0,!0)]),"doc-bottom":y(()=>[x(_.$slots,"doc-bottom",{},void 0,!0)]),"aside-top":y(()=>[x(_.$slots,"aside-top",{},void 0,!0)]),"aside-bottom":y(()=>[x(_.$slots,"aside-bottom",{},void 0,!0)]),"aside-outline-before":y(()=>[x(_.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":y(()=>[x(_.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":y(()=>[x(_.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":y(()=>[x(_.$slots,"aside-ads-after",{},void 0,!0)]),_:3}),E(no),x(_.$slots,"layout-bottom",{},void 0,!0)],2)):(i(),A(m,{key:1}))}}}),Va=q(qa,[["__scopeId","data-v-5d98c3a5"]]),rt={Layout:Va,enhanceApp:({app:e})=>{e.component("Badge",en)}},Ma={};function Aa(e,n){return x(e.$slots,"default")}const Da=q(Ma,[["render",Aa]]),Ba={class:"download-card"},Na={class:"platform-icon"},Fa={class:"version-badge"},Ra={class:"file-list"},Ha=["href","download"],za={class:"file-size"},Ua={key:0,class:"sha-line"},Ga={key:1,class:"install-cmd"},ja=["title"],Ja=w({__name:"DownloadCard",props:{icon:{},platform:{},version:{},arch:{},files:{},sha256:{},installCmd:{}},setup(e){const n=C(!1);function s(a){var o;(o=navigator.clipboard)==null||o.writeText(a),n.value=!0,setTimeout(()=>n.value=!1,1800)}return(a,o)=>(i(),l("div",Ba,[t("div",Na,v(e.icon),1),t("h3",null,v(e.platform),1),t("span",Fa,"v"+v(e.version)+" • "+v(e.arch),1),t("ul",Ra,[(i(!0),l(F,null,j(e.files,c=>(i(),l("li",{key:c.name},[t("a",{href:c.url,class:"download-btn",download:c.name},[Q(" ⬇ "+v(c.label)+" ",1),t("span",za,"("+v(c.size)+")",1)],8,Ha)]))),128))]),e.sha256?(i(),l("p",Ua,"SHA256: "+v(e.sha256),1)):g("",!0),e.installCmd?(i(),l("div",Ga,[t("code",null,v(e.installCmd),1),t("button",{class:"copy-btn",onClick:o[0]||(o[0]=c=>s(e.installCmd)),title:n.value?"تم النسخ!":"نسخ"},v(n.value?"✓":"⧉"),9,ja)])):g("",!0)]))}}),Qa=q(Ja,[["__scopeId","data-v-2c104e37"]]),Wa={class:"pg-examples"},Ka=["onClick"],Ya={class:"pg-workspace"},Xa={class:"pg-pane editor"},Za={class:"pg-header"},el={class:"pg-filename"},tl=["disabled"],nl={key:0},sl={key:1},ol={class:"pg-side"},il={class:"pg-pane stdin"},al={class:"pg-header"},ll={key:0,class:"pg-time"},cl={class:"pg-output-body"},rl={key:0,class:"pg-placeholder"},dl={key:1,class:"pg-error"},_l={key:2,class:"pg-out-text"},ul={key:0,class:"pg-loading-bar"},pl=w({__name:"CodePlayground",setup(e){const n=C(null),s=C(!1),a=C(!1),o=C([]),c=C(""),r=C(null),d=C(""),_=C("مرحباً"),p=C("مرحبا.ص"),m=C("loading"),h=O(()=>!!c.value);let S=null,f=null;const b=[{label:"مرحباً",icon:"👋",filename:"مرحبا.ص",code:`# مرحبا بالعالم!
اطبع_سطر("مرحباً يا عالم! 🌍")
اطبع_سطر("أهلاً بلغة ص — لغة البرمجة العربية")
اطبع_سطر("الإصدار: 1.0.0")
`},{label:"الفيبوناتشي",icon:"🔢",filename:"فيبو.ص",code:`دالة فيبو(ن)
  إذا (ن <= 1)  ارجع ن  نهاية
  ارجع فيبو(ن - 1) + فيبو(ن - 2)
نهاية

اطبع_سطر("متتالية فيبوناتشي:")
لكل ي في مدى(0، 12)
  اطبع(فيبو(ي) + "  ")
نهاية
`},{label:"الأصناف",icon:"🏗️",filename:"أصناف.ص",code:`صنف حيوان
  باني(الاسم، الصوت)
    هذا.الاسم  = الاسم
    هذا.الصوت = الصوت
  نهاية
  دالة تحدث()
    اطبع_سطر(هذا.الاسم + " يقول: " + هذا.الصوت + "!")
  نهاية
نهاية

صنف كلب يرث حيوان
  باني(الاسم)  الأساس(الاسم، "هاو")  نهاية
نهاية

صنف قطة يرث حيوان
  باني(الاسم)  الأساس(الاسم، "مياو")  نهاية
نهاية

متغير حيوانات = [جديد كلب("بوبي")، جديد قطة("مشمش")]
لكل حيوان في حيوانات
  حيوان.تحدث()
نهاية
`},{label:"الترتيب",icon:"📊",filename:"ترتيب.ص",code:`# خوارزمية فرز الفقاعات
دالة فرز_فقاعي(مصفوفة)
  متغير حجم = مصفوفة.طول()
  لكل ي في مدى(0، حجم)
    لكل ج في مدى(0، حجم - ي - 1)
      إذا (مصفوفة[ج] > مصفوفة[ج + 1])
        متغير مؤقت = مصفوفة[ج]
        مصفوفة[ج] = مصفوفة[ج + 1]
        مصفوفة[ج + 1] = مؤقت
      نهاية
    نهاية
  نهاية
  ارجع مصفوفة
نهاية

متغير أرقام = [64، 34، 25، 12، 22، 11، 90]
اطبع_سطر("قبل الترتيب: " + أرقام)
متغير مرتبة = فرز_فقاعي(أرقام)
اطبع_سطر("بعد الترتيب: " + مرتبة)
`},{label:"معالجة الأخطاء",icon:"🛡️",filename:"أخطاء.ص",code:`دالة قسمة_آمنة(أ، ب)
  حاول
    إذا (ب == 0)
      ارمي("خطأ: لا يمكن القسمة على صفر!")
    نهاية
    ارجع أ / ب
  امسك خطأ
    اطبع_سطر("تم التقاط خطأ: " + خطأ)
    ارجع لاشيء
  نهاية
نهاية

اطبع_سطر(قسمة_آمنة(10، 2))
اطبع_سطر(قسمة_آمنة(5، 0))
اطبع_سطر(قسمة_آمنة(9، 3))
`},{label:"async/await",icon:"⚡",filename:"غيرمتزامن.ص",code:`غير_متزامن دالة جلب_بيانات(معرف)
  اطبع_سطر("جارٍ جلب البيانات لـ " + معرف + "...")
  انتظر تأخير(100)
  ارجع "بيانات المستخدم #" + معرف
نهاية

غير_متزامن دالة رئيسية()
  متغير بيانات = انتظر جلب_بيانات(42)
  اطبع_سطر("النتيجة: " + بيانات)
نهاية

رئيسية()
`}];function I(T){_.value=T.label,p.value=T.filename,f&&f.dispatch({changes:{from:0,to:f.state.doc.length,insert:T.code}}),M()}function M(){o.value=[],c.value="",r.value=null}async function H(){if(!f||a.value)return;const T=f.state.doc.toString(),k=d.value;a.value=!0,o.value=[],c.value="",r.value=null;const $=performance.now();try{let D;if(S&&m.value==="wasm")try{const K=S.ccall("sad_execute","number",["string","string"],[T,k]),G=S.UTF8ToString(K);S._free(K),D=JSON.parse(G)}catch{D=await V(T,k)}else D=await V(T,k);r.value=Math.round(performance.now()-$),!D.success&&D.error?c.value=D.error:o.value=D.output.split(`
`)}catch(D){c.value=`خطأ في التنفيذ: ${(D==null?void 0:D.message)??D}`,r.value=Math.round(performance.now()-$)}finally{a.value=!1}}async function V(T,k){const $=new AbortController,D=setTimeout(()=>$.abort(),1e4);try{const K=await fetch("http://localhost:3731/execute",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({source:T,stdin:k}),signal:$.signal});return clearTimeout(D),await K.json()}catch{return clearTimeout(D),{success:!1,output:"",error:["⚠️ لتشغيل الكود في المتصفح، ابدأ خادم API المحلي:","","  node website/api/server.js","","أو ابنِ sad.wasm عبر Emscripten:","  انظر website/build-wasm.ps1"].join(`
`)}}}async function P(){if(n.value)try{const[{EditorState:T},{EditorView:k,keymap:$,lineNumbers:D,highlightActiveLineGutter:K,highlightSpecialChars:G,drawSelection:J,dropCursor:z,rectangularSelection:ie,crosshairCursor:pe,highlightActiveLine:ve},{defaultKeymap:he,historyKeymap:fe,history:ae},{indentOnInput:be,syntaxHighlighting:ye,defaultHighlightStyle:me,bracketMatching:$e},{closeBrackets:ge,closeBracketsKeymap:ze},{oneDark:Ue}]=await Promise.all([Z(()=>import("./index.tqIctuLa.js"),[]),Z(()=>import("./index.BPH623Bm.js").then(Se=>Se.i),__vite__mapDeps([2,3])),Z(()=>import("./index.CyGSWCqK.js"),__vite__mapDeps([4,3,2,5])),Z(()=>import("./index.DGTKRUWM.js").then(Se=>Se.d),__vite__mapDeps([5,3,2])),Z(()=>import("./index.pqLJMZUs.js"),__vite__mapDeps([6,3,2,5])),Z(()=>import("./index.D9sbPGjv.js"),__vite__mapDeps([7,2,3,5]))]);f=new k({state:T.create({doc:b[0].code,extensions:[D(),K(),G(),ae(),J(),z(),ie(),pe(),be(),ye(me,{fallback:!0}),$e(),ge(),ve(),$.of([...ze,...he,...fe,{key:"Ctrl-Enter",run:()=>(H(),!0)}]),Ue,k.theme({"&":{height:"100%",fontSize:"14px"},".cm-content":{direction:"ltr",textAlign:"left",paddingTop:"10px"},".cm-scroller":{overflow:"auto",fontFamily:'"JetBrains Mono", monospace'}})]}),parent:n.value})}catch{const T=document.createElement("textarea");T.value=b[0].code,T.style.cssText=`width:100%;height:100%;background:#0d1117;color:#e6edf3;
      padding:12px;font-family:monospace;font-size:14px;border:none;resize:none;
      direction:ltr;box-sizing:border-box;`,n.value.appendChild(T),f={state:{doc:{toString:()=>T.value,length:T.value.length}},dispatch:({changes:k})=>{(k==null?void 0:k.insert)!==void 0&&(T.value=k.insert)}}}}async function L(){try{const T=document.createElement("script");T.src="/sad.js",await new Promise(($,D)=>{T.onload=()=>$(),T.onerror=()=>D(),document.head.appendChild(T)});const k=window.SadWasm;S=await k(),m.value="wasm"}catch{m.value="api"}s.value=!0}return ee(async()=>{await P(),await L()}),de(()=>{var T;(T=f==null?void 0:f.destroy)==null||T.call(f)}),(T,k)=>(i(),l(F,null,[t("div",Wa,[k[1]||(k[1]=t("span",{class:"pg-examples-label"},"أمثلة جاهزة:",-1)),(i(),l(F,null,j(b,$=>t("button",{key:$.label,class:N(["pg-tab",{active:_.value===$.label}]),onClick:D=>I($)},v($.icon)+" "+v($.label),11,Ka)),64))]),t("div",Ya,[t("div",Xa,[t("div",Za,[k[2]||(k[2]=t("span",null,"📝 المحرر",-1)),t("span",el,v(p.value),1),t("div",{class:N(["pg-mode-badge",m.value])},v(m.value==="wasm"?"⚡ WASM":m.value==="api"?"🔌 API":"⏳"),3),t("button",{class:"pg-run-btn",onClick:H,disabled:a.value},[a.value?(i(),l("span",nl,"⏳ جارٍ التنفيذ...")):(i(),l("span",sl,"▶ تشغيل"))],8,tl)]),t("div",{ref_key:"editorEl",ref:n,class:"pg-editor-mount"},null,512)]),t("div",ol,[t("div",il,[k[3]||(k[3]=t("div",{class:"pg-header"},[t("span",null,"⌨️ إدخال stdin "),t("span",{class:"pg-hint-small"},"(اختياري)")],-1)),Re(t("textarea",{"onUpdate:modelValue":k[0]||(k[0]=$=>d.value=$),class:"pg-stdin",placeholder:"أدخل قيمة اقرأ() هنا...",dir:"auto",spellcheck:"false"},null,512),[[et,d.value]])]),t("div",{class:N(["pg-pane output",{"has-error":h.value}])},[t("div",al,[k[4]||(k[4]=t("span",null,"📤 الخرج",-1)),r.value!==null?(i(),l("span",ll,v(r.value)+"ms",1)):g("",!0),t("button",{class:"pg-clear",onClick:M},"×")]),t("div",cl,[o.value.length===0&&!h.value?(i(),l("div",rl,' اضغط "تشغيل" لتنفيذ الكود ✨ ')):g("",!0),h.value?(i(),l("div",dl,[k[5]||(k[5]=t("span",{class:"pg-error-icon"},"❌",-1)),t("pre",null,v(c.value),1)])):g("",!0),o.value.length?(i(),l("pre",_l,v(o.value.join(`
`)),1)):g("",!0)])],2)])]),s.value?g("",!0):(i(),l("div",ul,[...k[6]||(k[6]=[t("div",{class:"pg-loading-fill"},null,-1),t("span",null,"جارٍ تحميل مفسر لغة ص...",-1)])]))],64))}}),vl=q(pl,[["__scopeId","data-v-1baeb693"]]),hl={class:"feature-grid"},fl={class:"feat-icon"},ml={class:"feat-title"},gl={class:"feat-desc"},xl=w({__name:"FeatureGrid",props:{features:{}},setup(e){return(n,s)=>(i(),l("div",hl,[(i(!0),l(F,null,j(e.features,a=>(i(),l("div",{key:a.title,class:"feature-item"},[t("div",fl,v(a.icon),1),t("h3",ml,v(a.title),1),t("p",gl,v(a.desc),1)]))),128))]))}}),kl=q(xl,[["__scopeId","data-v-37d4be52"]]),bl={class:"sad-version"},yl="1.0.0",$l=w({__name:"SadVersion",setup(e){return(n,s)=>(i(),l("div",bl,[s[0]||(s[0]=t("span",{class:"version-label"},"الإصدار",-1)),t("a",{class:"version-tag",href:"https://github.com/sad-lang/sad-language/releases",target:"_blank",rel:"noopener"}," v"+v(yl))]))}}),Sl=q($l,[["__scopeId","data-v-d768f152"]]),Cl={class:"tutorial-container"},Ll={class:"tutorial-progress"},Pl={class:"progress-text"},Tl={class:"step-tabs"},Ol=["onClick"],Il={class:"step-num"},wl={class:"step-label"},El={class:"step-content"},ql={class:"step-explanation"},Vl={class:"step-title"},Ml=["innerHTML"],Al={key:0,class:"step-hint"},Dl={class:"step-editor"},Bl={class:"editor-toolbar"},Nl={class:"file-badge"},Fl=["disabled"],Rl={key:0,class:"output-hint"},Hl={key:1,class:"output-error"},zl={key:2},Ul={class:"step-nav"},Gl=["disabled"],jl={class:"step-dots"},Jl=["onClick"],Ql=w({__name:"InteractiveTutorial",props:{steps:{}},emits:["finished"],setup(e,{emit:n}){const s=e,a=C(0),o=C(!1),c=C(!1),r=C(""),d=C(""),_=C(null);let p=null;const m=O(()=>s.steps[a.value]),h=O(()=>a.value/(s.steps.length-1)*100),S=O(()=>!!d.value);function f(P){a.value=P,o.value=!1,r.value="",d.value=""}function b(){a.value<s.steps.length-1&&f(a.value+1)}function I(){a.value>0&&f(a.value-1)}function M(){p&&p.dispatch({changes:{from:0,to:p.state.doc.length,insert:m.value.code}})}async function H(){if(!p)return;const P=p.state.doc.toString();c.value=!0,r.value="",d.value="";try{const L=window.__sadWasm;if(L!=null&&L.runCode){const T=L.runCode(P);T.error?d.value=T.error:r.value=T.output}else{const k=await(await fetch("/api/run",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({code:P})})).json();k.error?d.value=k.error:r.value=k.output??""}}catch(L){d.value=`خطأ: ${L==null?void 0:L.message}`}finally{c.value=!1}}ne(a,async()=>{await V()});async function V(){var P;if(_.value){(P=p==null?void 0:p.destroy)==null||P.call(p),_.value.innerHTML="";try{const[{EditorState:L},{EditorView:T,keymap:k,lineNumbers:$,highlightActiveLine:D},{defaultKeymap:K,history:G,historyKeymap:J},{indentOnInput:z,syntaxHighlighting:ie,defaultHighlightStyle:pe},{closeBrackets:ve,closeBracketsKeymap:he},{oneDark:fe}]=await Promise.all([Z(()=>import("./index.tqIctuLa.js"),[]),Z(()=>import("./index.BPH623Bm.js").then(ae=>ae.i),__vite__mapDeps([2,3])),Z(()=>import("./index.CyGSWCqK.js"),__vite__mapDeps([4,3,2,5])),Z(()=>import("./index.DGTKRUWM.js").then(ae=>ae.d),__vite__mapDeps([5,3,2])),Z(()=>import("./index.pqLJMZUs.js"),__vite__mapDeps([6,3,2,5])),Z(()=>import("./index.D9sbPGjv.js"),__vite__mapDeps([7,2,3,5]))]);p=new T({state:L.create({doc:m.value.code,extensions:[$(),D(),G(),z(),ie(pe,{fallback:!0}),ve(),k.of([...he,...K,...J]),fe,T.theme({"&":{height:"200px",fontSize:"13px"},".cm-content":{direction:"ltr",textAlign:"left"},".cm-scroller":{overflow:"auto",fontFamily:"JetBrains Mono, monospace"}})]}),parent:_.value})}catch{const L=document.createElement("textarea");L.value=m.value.code,L.style.cssText="width:100%;height:200px;background:#0d1117;color:#e6edf3;padding:10px;font-family:monospace;font-size:13px;border:none;resize:none;direction:ltr;",_.value.appendChild(L),p={state:{doc:{toString:()=>L.value,length:L.value.length}},dispatch({changes:T}){T.insert!==void 0&&(L.value=T.insert)},destroy(){}},L.addEventListener("input",()=>{p.state.doc={toString:()=>L.value,length:L.value.length}})}}}return ee(V),de(()=>{var P;return(P=p==null?void 0:p.destroy)==null?void 0:P.call(p)}),(P,L)=>(i(),l("div",Cl,[t("div",Ll,[t("div",{class:"progress-bar",style:W({width:h.value+"%"})},null,4),t("span",Pl," الخطوة "+v(a.value+1)+" من "+v(e.steps.length),1)]),t("div",Tl,[(i(!0),l(F,null,j(e.steps,(T,k)=>(i(),l("button",{key:k,class:N(["step-tab",{active:k===a.value,completed:k<a.value}]),onClick:$=>f(k)},[t("span",Il,v(k+1),1),t("span",wl,v(T.title),1)],10,Ol))),128))]),t("div",El,[t("div",ql,[t("h3",Vl,v(m.value.title),1),t("div",{class:"step-desc",innerHTML:m.value.descHtml},null,8,Ml),m.value.hint&&o.value?(i(),l("div",Al," 💡 "+v(m.value.hint),1)):g("",!0),m.value.hint&&!o.value?(i(),l("button",{key:1,class:"hint-btn",onClick:L[0]||(L[0]=T=>o.value=!0)}," 💡 عرض تلميح ")):g("",!0)]),t("div",Dl,[t("div",Bl,[t("span",Nl,v(m.value.filename||"تمرين.ص"),1),t("button",{class:"reset-btn",onClick:M,title:"إعادة الكود الأصلي"},"↺"),t("button",{class:"run-step-btn",onClick:H,disabled:c.value},v(c.value?"⏳":"▶ تشغيل"),9,Fl)]),t("div",{ref_key:"stepEditorEl",ref:_,class:"step-editor-mount"},null,512),t("div",{class:N(["step-output",{error:S.value}])},[!r.value&&!d.value?(i(),l("span",Rl,"الخرج سيظهر هنا...")):g("",!0),d.value?(i(),l("span",Hl,v(d.value),1)):g("",!0),r.value?(i(),l("span",zl,v(r.value),1)):g("",!0)],2)])]),t("div",Ul,[t("button",{class:"nav-btn prev",disabled:a.value===0,onClick:I}," ← السابق ",8,Gl),t("div",jl,[(i(!0),l(F,null,j(e.steps,(T,k)=>(i(),l("span",{key:k,class:N(["dot",{active:k===a.value,done:k<a.value}]),onClick:$=>f(k)},null,10,Jl))),128))]),a.value<e.steps.length-1?(i(),l("button",{key:0,class:"nav-btn next",onClick:b}," التالي → ")):(i(),l("button",{key:1,class:"nav-btn finish",onClick:L[1]||(L[1]=T=>P.$emit("finished"))}," ✓ إنهاء الدرس "))])]))}}),Wl=q(Ql,[["__scopeId","data-v-56f45a92"]]),Kl={id:"l1_s1",title:"أول خطوات",icon:"👣",lessons:[{id:"l1_01",title:"مرحبا بالعالم!",desc:"اكتب أول سطر كود — برنامج مرحبا بالعالم الكلاسيكي",explanation:'في عالم البرمجة، يُعتبر برنامج "مرحبا بالعالم" أول برنامج يكتبه أي مبرمج عند تعلّم لغة جديدة. هذا التقليد بدأ منذ عقود ولا يزال مستمراً لأنه أبسط طريقة للتأكد من أن بيئة البرمجة تعمل بشكل صحيح. عندما ترى النص يظهر على الشاشة، تعرف أن كل شيء جاهز للبدء!\n\nفي لغة ص، نستخدم الدالة `اطبع_سطر()` لعرض النص على الشاشة. هذه الدالة تأخذ النص المراد طباعته بين علامتي تنصيص "" وتعرضه ثم تنتقل تلقائياً إلى سطر جديد. هناك أيضاً الدالة `اطبع()` التي تعرض النص بدون الانتقال لسطر جديد — سنتعلمها قريباً.\n\nلاحظ أن النص يجب أن يكون دائماً بين علامتي تنصيص مزدوجة "". هذا يخبر اللغة أن ما بداخل التنصيص هو نص حرفي وليس اسم متغير أو أمر برمجي. السطر الذي يبدأ بـ `#` هو تعليق — ملاحظة للمبرمج لا يُنفّذها الحاسوب.\n\nتهانينا! بكتابة هذا البرنامج البسيط، أنت الآن مبرمج رسمياً. كل البرامج المعقدة التي تراها — من التطبيقات إلى الألعاب — بدأت بخطوة بسيطة مثل هذه.',code:`# أول برنامج لك بلغة ص!
اطبع_سطر("مرحبا بالعالم!")`,exercise:`# تمرين: اطبع عبارة ترحيب خاصة بك
# مثال: "أهلاً، أنا أحمد!"
`,hint:'استخدم اطبع_سطر("النص هنا")',tips:["اطبع_سطر تطبع النص ثم تنتقل لسطر جديد","اطبع بدون _سطر لا تنتقل لسطر جديد",'النصوص دائماً بين علامتي تنصيص ""'],concepts:["طباعة","اطبع_سطر"]},{id:"l1_02",title:"التعليقات — وثّق كودك",desc:"التعليقات ملاحظات تُكتب للمبرمج ولا تُنفَّذ",explanation:'التعليقات هي ملاحظات يكتبها المبرمج داخل الكود لشرح ما يفعله البرنامج أو لتذكير نفسه بشيء مهم. الحاسوب يتجاهل التعليقات تماماً ولا يُنفّذها — فهي مكتوبة للبشر فقط. تخيّل أنك تكتب ملاحظات على هامش كتاب: الكتاب لا يتغير، لكن ملاحظاتك تساعدك على الفهم لاحقاً.\n\nفي لغة ص، هناك ثلاثة أنواع من التعليقات: تعليق السطر الواحد يبدأ بعلامة `#` وكل ما بعدها في نفس السطر يُعتبر تعليقاً. تعليق الكتلة يبدأ بـ `#*` وينتهي بـ `*#` ويمكن أن يمتد عبر عدة أسطر — مفيد لشرح مقطع كامل من الكود. تعليق التوثيق يبدأ بـ `##` أو `#** ... **#` ويُستخدم لتوليد وثائق تلقائية لوظائف البرنامج.\n\nالقاعدة الذهبية في كتابة التعليقات: اشرح "لماذا" وليس "ماذا". الكود الجيد يشرح نفسه بنفسه، لكن التعليقات توضّح السبب وراء القرارات. مثلاً، بدلاً من كتابة "# نضرب في 100" (واضح من الكود)، اكتب "# نحوّل النسبة المئوية إلى عدد صحيح للدقة".\n\nالتعليقات مهمة جداً عند العمل ضمن فريق، فهي تساعد زملاءك على فهم كودك بسرعة. حتى لو كنت تعمل وحدك، ستشكر نفسك بعد أشهر عندما تعود لكود قديم وتجد تعليقات واضحة تشرح ما كنت تفكر فيه.',code:`# هذا تعليق سطر واحد — لن يُنفَّذ

اطبع_سطر("هذا سيُنفَّذ")  # ويمكن كتابته بعد الكود

#*
  هذا تعليق
  متعدد الأسطر (كتلة)
  مفيد للشروحات الطويلة
*#

## هذا تعليق توثيقي — يظهر في التوثيق التلقائي
اطبع_سطر("تعلّمنا التعليقات!")`,exercise:`# تمرين: اكتب 3 أنواع من التعليقات
# 1. تعليق سطر
# 2. تعليق كتلة
# 3. تعليق توثيقي
# ثم اطبع "فهمت التعليقات!"
`,hint:"استخدم # للسطر و #* ... *# للكتلة و ## للتوثيق",tips:["التعليقات الجيدة تشرح لماذا وليس ماذا","تعليقات التوثيق ## تُستخدم لإنشاء وثائق API تلقائية"],concepts:["تعليقات","#","#*","##"]},{id:"l1_03",title:"الطباعة المتقدمة",desc:"تعلّم طرق الطباعة المختلفة والتحكم في الخرج",explanation:"الطباعة هي الطريقة الأساسية التي يتواصل بها برنامجك مع المستخدم. في لغة ص، لديك أداتان رئيسيتان: `اطبع_سطر()` التي تطبع النص ثم تنتقل لسطر جديد، و`اطبع()` التي تطبع النص وتبقى في نفس السطر. الفرق بينهما مهم لأنه يتحكم في شكل المخرجات.\n\nعندما تستخدم `اطبع()` عدة مرات متتالية، كل النصوص تظهر على نفس السطر. هذا مفيد عندما تريد بناء سطر من عدة أجزاء. مثلاً، يمكنك طباعة اسم المستخدم ثم ترحيب مخصص على نفس السطر. أما `اطبع_سطر()` فكل استدعاء يبدأ سطراً جديداً بعد الطباعة.\n\nيمكنك دمج النصوص مع الأرقام باستخدام عامل الجمع `+`. عندما تجمع نصاً مع رقم، اللغة تحوّل الرقم تلقائياً إلى نص وتدمجهما. لاحظ أنه عند حساب تعبير رياضي داخل جملة طباعة، يجب وضعه بين أقواس `()` حتى يُحسب أولاً قبل الدمج مع النص.\n\nإتقان التحكم في المخرجات هو مهارة أساسية ستحتاجها في كل برنامج تكتبه — من عرض نتائج الحسابات إلى تنسيق التقارير وبناء واجهات نصية جميلة.",code:`# الطباعة مع سطر جديد
اطبع_سطر("السطر الأول")
اطبع_سطر("السطر الثاني")

# الطباعة بدون سطر جديد
اطبع("مرحبا ")
اطبع("يا ")
اطبع_سطر("عالم!")

# طباعة قيم متعددة
اطبع_سطر("الناتج: " + 42)
اطبع_سطر("مجموع: " + (10 + 20))`,exercise:`# تمرين: اطبع بطاقتك الشخصية بهذا الشكل:
# ╔══════════════╗
# ║  اسمك هنا   ║
# ║  عمرك هنا   ║
# ╚══════════════╝
`,hint:"استخدم اطبع_سطر لكل سطر من البطاقة",tips:["يمكنك دمج نص مع رقم باستخدام +","الأقواس () تحدد ترتيب العمليات الحسابية"],concepts:["اطبع","اطبع_سطر","دمج النصوص"]},{id:"l1_04",title:"قراءة الإدخال من المستخدم",desc:"البرامج التفاعلية تقرأ إدخال المستخدم",explanation:'البرامج الحقيقية لا تعمل بمعزل عن المستخدم — بل تحتاج للتفاعل معه. قراءة الإدخال تعني أن البرنامج ينتظر حتى يكتب المستخدم شيئاً ويضغط Enter، ثم يخزّن ما كتبه ليستخدمه لاحقاً. هكذا تصنع برامج تفاعلية تستجيب لاحتياجات كل مستخدم.\n\nالدالة `اقرأ()` في لغة ص تنتظر إدخال المستخدم وتُرجع ما كتبه كنص. يمكنك تخزين هذا النص في متغير لاستخدامه لاحقاً. النمط الشائع هو: أولاً تطبع سؤالاً باستخدام `اطبع()`، ثم تقرأ الإجابة بـ `اقرأ()` وتخزّنها في متغير.\n\nنقطة مهمة جداً: `اقرأ()` تُرجع دائماً نصاً، حتى لو أدخل المستخدم رقماً. يعني لو المستخدم كتب "25"، القيمة المُخزّنة ستكون النص "25" وليس الرقم 25. سنتعلم لاحقاً كيف نحوّل النص إلى رقم باستخدام دوال التحويل.\n\nبفضل `اقرأ()`، يمكنك بناء برامج متنوعة: آلة حاسبة تسأل المستخدم عن الأرقام، لعبة تسأل عن الإجابات، أو برنامج تسجيل يجمع معلومات المستخدم.',code:`# قراءة إدخال المستخدم
اطبع("ما اسمك؟ ")
متغير الاسم = اقرأ()
اطبع_سطر("مرحبا يا " + الاسم + "!")

# قراءة مع رسالة
اطبع("كم عمرك؟ ")
متغير العمر = اقرأ()
اطبع_سطر("عمرك " + العمر + " سنة")`,exercise:`# تمرين: اسأل المستخدم عن لونه المفضل ثم اطبع:
# "لونك المفضل هو: ..."
`,hint:"استخدم اقرأ() لأخذ الإدخال ثم ادمجه مع نص",tips:["اقرأ() ترجع دائماً نصاً — سنتعلم تحويله لرقم لاحقاً",'يمكنك تمرير رسالة: اقرأ("رسالة")'],concepts:["اقرأ","إدخال المستخدم"]},{id:"l1_05",title:"المتغيرات — تخزين القيم",desc:"المتغيرات صناديق تخزّن فيها بياناتك",explanation:'المتغير هو كصندوق مُسمّى تضع فيه قيمة لتستخدمها لاحقاً. تخيّل أن لديك صناديق مكتوب عليها "الاسم" و"العمر" و"الطول" — كل صندوق يحتوي على قيمة مختلفة ويمكنك الرجوع إليه متى شئت باستخدام اسمه. هذا بالضبط ما يفعله المتغير في البرمجة.\n\nلتعريف متغير في لغة ص، استخدم الكلمة المفتاحية `متغير` متبوعة باسم المتغير ثم علامة `=` ثم القيمة. مثلاً: `متغير الاسم = "سارة"`. اللغة ذكية بما يكفي لتعرف نوع القيمة تلقائياً — فلا حاجة لتحديد أن "سارة" نص أو أن 22 رقم.\n\nالميزة الأساسية للمتغير هي أن قيمته يمكن تغييرها في أي وقت. مثلاً، يمكنك تعريف `متغير عداد = 0` ثم تغييره إلى `عداد = 10`. لاحظ أنك عند تغيير القيمة لا تكتب `متغير` مرة أخرى — الكلمة `متغير` تُستخدم فقط عند التعريف لأول مرة.\n\nالمتغيرات هي اللبنة الأساسية لكل برنامج. بدونها لا يمكنك تخزين بيانات المستخدم، أو حفظ نتائج الحسابات، أو تتبع حالة البرنامج. ستستخدمها في كل سطر كود تقريباً من الآن فصاعداً.',code:`# تعريف متغيرات مختلفة
متغير الاسم = "سارة"
متغير العمر = 22
متغير الطول = 1.65
متغير طالبة = صحيح

# استخدام المتغيرات
اطبع_سطر("الاسم: " + الاسم)
اطبع_سطر("العمر: " + العمر)

# تغيير قيمة متغير
متغير عداد = 0
اطبع_سطر("قبل: " + عداد)
عداد = 10
اطبع_سطر("بعد: " + عداد)`,exercise:`# تمرين: عرّف 4 متغيرات تصف مدينتك:
# اسم المدينة، عدد السكان، المساحة، هل هي عاصمة
# ثم اطبعها
`,hint:"استخدم متغير لكل معلومة واختر النوع المناسب",tips:["المتغير يمكن تغيير قيمته لاحقاً","اللغة تستنتج نوع المتغير تلقائياً","الأسماء العربية مدعومة بالكامل"],concepts:["متغير","تعريف","إسناد"]},{id:"l1_06",title:"أسماء المتغيرات — القواعد",desc:"قواعد تسمية المتغيرات بالعربية والإنجليزية",explanation:"اختيار اسم جيد للمتغير هو فن بحد ذاته. الاسم الجيد يجعل الكود يُقرأ كنص عربي طبيعي، بينما الاسم السيء يجعلك تتوقف عند كل سطر لتتذكر ماذا يعني. تخيّل الفرق بين قراءة `عدد_الطلاب = 30` وقراءة `س = 30` — الأول واضح تماماً!\n\nلغة ص تدعم الأسماء العربية بالكامل، وهذه ميزة رائعة تجعل الكود مفهوماً لأي شخص يقرأ العربية. قواعد التسمية بسيطة: الاسم يجب أن يبدأ بحرف (عربي أو إنجليزي) أو بشرطة سفلية `_`، ويمكن أن يحتوي على حروف وأرقام وشرطات سفلية. لا يمكن أن يبدأ برقم أو يحتوي على مسافات أو شرطات عادية.\n\nأفضل الممارسات في تسمية المتغيرات: استخدم أسماء وصفية تشرح محتوى المتغير مثل `عدد_صفحات_الكتاب` بدلاً من `ع`. استخدم الشرطة السفلية `_` للفصل بين الكلمات. تجنب الأسماء القصيرة جداً إلا في حالات خاصة مثل عدادات الحلقات. ولا تستخدم الكلمات المحجوزة مثل `متغير` أو `دالة` كأسماء.\n\nتذكّر: أنت تكتب الكود مرة واحدة لكنك تقرأه مئات المرات. الوقت الذي تقضيه في اختيار اسم واضح يوفّر عليك ساعات من البحث والتذكر لاحقاً.",code:`# أسماء عربية — مدعومة بالكامل!
متغير عدد_الطلاب = 30
متغير المدرسة = "الفلاح"
متغير نسبة_النجاح = 95.5

# أسماء إنجليزية — مدعومة أيضاً
متغير count = 100
متغير name = "Ali"

# أسماء مختلطة
متغير طالب1 = "محمد"
متغير student_2 = "أحمد"

# أسماء وصفية — الأفضل!
متغير عدد_صفحات_الكتاب = 350
اطبع_سطر("الطلاب: " + عدد_الطلاب)
اطبع_سطر("عدد الصفحات: " + عدد_صفحات_الكتاب)`,exercise:`# تمرين: أيّ الأسماء التالية صحيحة؟ جرّب كلاً منها:
# متغير 1عدد = 10            (يبدأ برقم — خطأ!)
# متغير عدد_طلاب = 10       (صحيح ✓)
# متغير عدد طلاب = 10       (فراغ — خطأ!)
# متغير العدد-الكلي = 10    (شرطة — خطأ!)
`,hint:"الأسماء تبدأ بحرف أو _ ، وتحتوي حروف وأرقام و _ فقط",tips:["الأسماء العربية الوصفية تجعل الكود أوضح","استخدم _ للفصل بين الكلمات: عدد_الطلاب","تجنب الأسماء القصيرة جداً مثل ع أو س إلا في الحلقات"],concepts:["تسمية المتغيرات","UTF-8"]},{id:"l1_07",title:"الثوابت — قيم لا تتغير",desc:"الثوابت قيم تُحدد مرة واحدة ولا تتغير أبداً",explanation:"الثابت يشبه المتغير تماماً من حيث تخزين القيم، لكن مع فارق جوهري واحد: قيمة الثابت لا يمكن تغييرها بعد تعريفها. تخيّل أنه صندوق مقفل — تضع فيه القيمة مرة واحدة ثم لا يمكنك تغييرها. هذا يحميك من تغيير قيم حساسة بالخطأ.\n\nلتعريف ثابت في لغة ص، استخدم الكلمة `ثابت` بدلاً من `متغير`. مثلاً: `ثابت ثابت_باي = 3.14159`. إذا حاولت تغيير قيمته لاحقاً بكتابة `ثابت_باي = 3.14`، ستحصل على خطأ. هذا مقصود — اللغة تحميك من الأخطاء غير المقصودة.\n\nمتى تستخدم `ثابت` بدلاً من `متغير`؟ استخدمه لأي قيمة تعرف أنها لن تتغير أثناء تشغيل البرنامج: القيم الرياضية مثل π، حدود النظام مثل السرعة القصوى، إعدادات التطبيق مثل اسمه. القاعدة الجيدة: ابدأ دائماً بـ `ثابت` ولا تستخدم `متغير` إلا إذا كنت متأكداً أن القيمة ستتغير.\n\nاستخدام الثوابت يجعل الكود أكثر أماناً وقابلية للصيانة. عندما يقرأ شخص آخر كودك ويرى `ثابت`، يعرف فوراً أن هذه القيمة مقدسة ولا يجب المساس بها.",code:`# تعريف ثوابت — لا يمكن تغييرها لاحقاً
ثابت ثابت_باي = 3.14159
ثابت السرعة_القصوى = 120
ثابت اسم_التطبيق = "تطبيقي"

اطبع_سطر("π = " + ثابت_باي)
اطبع_سطر("السرعة القصوى: " + السرعة_القصوى + " كم/س")
اطبع_سطر("التطبيق: " + اسم_التطبيق)

# هذا سيسبب خطأ:
# ثابت_باي = 3.14    # ❌ لا يمكن تغيير ثابت!

# مقارنة مع المتغير:
متغير سرعة_حالية = 60
سرعة_حالية = 80     # ✓ يمكن تغيير المتغير`,exercise:`# تمرين: عرّف ثوابت لتطبيق مدرسي:
# - اسم المدرسة
# - عدد الحصص اليومية
# - مدة الحصة بالدقائق
# ثم احسب واطبع إجمالي ساعات اليوم الدراسي
`,hint:"استخدم ثابت بدل متغير لتعريف القيم الثابتة",tips:["استخدم ثابت للقيم التي لن تتغير مطلقاً","الثوابت تحمي من الأخطاء غير المقصودة","اكتب أسماء الثوابت بأحرف واضحة مثل ثابت_باي"],concepts:["ثابت","قيم غير قابلة للتغيير"]},{id:"l1_08",title:"أنواع البيانات الأساسية",desc:"تعرّف على الأنواع الأربعة الأساسية في لغة ص",explanation:'كل قيمة في البرمجة لها "نوع" يحدد ما يمكنك فعله بها. تماماً كما في الحياة: لا يمكنك جمع تفاحة مع رقم هاتف! الأنواع الأساسية في لغة ص هي أربعة: `رقم` للأعداد الصحيحة، `عشري` للأعداد العشرية، `نص` للسلاسل النصية، و`منطقي` للقيم المنطقية (صحيح/خطأ).\n\nالنوع `رقم` يمثل الأعداد الصحيحة مثل 25 و-10 و0 — أي عدد بدون فاصلة عشرية. النوع `عشري` يمثل الأرقام ذات الفاصلة العشرية مثل 3.14 و1.75. النوع `نص` يمثل أي نص محاط بعلامات تنصيص مثل "مرحبا"، ويمكن أن يحتوي على حروف عربية وإنجليزية وأرقام ورموز. النوع `منطقي` له قيمتان فقط: `صحيح` و`خطأ` — مثالي لتمثيل حالات نعم/لا.\n\nلغة ص تستنتج النوع تلقائياً من القيمة التي تُسندها للمتغير — فلا حاجة لتحديده يدوياً. يمكنك دائماً معرفة نوع أي قيمة باستخدام الدالة `نوع()`. مثلاً، `نوع(42)` تُرجع "رقم" و`نوع("مرحبا")` تُرجع "نص".\n\nفهم الأنواع ضروري لتجنب الأخطاء. مثلاً، جمع رقمين يُنتج رقماً (5 + 3 = 8)، لكن "جمع" نصين يدمجهما ("مرحبا" + " يا" = "مرحبا يا"). هناك أيضاً القيمة الخاصة `لاشيء` التي تمثل "لا قيمة" — سنتعرف عليها أكثر لاحقاً.',code:`# النوع 1: رقم — أعداد صحيحة (integer)
متغير عمر = 25
متغير درجة_حرارة = -5
اطبع_سطر("رقم: " + عمر)

# النوع 2: عشري — أعداد عشرية (float)
متغير طول = 1.75
متغير سعر = 29.99
اطبع_سطر("عشري: " + طول)

# النوع 3: نص — سلسلة نصية (string)
متغير اسم = "محمد العربي"
متغير تحية = "مرحبا!"
اطبع_سطر("نص: " + اسم)

# النوع 4: منطقي — صحيح أو خطأ (boolean)
متغير نشط = صحيح
متغير منتهي = خطأ
اطبع_سطر("منطقي: " + نشط)

# معرفة نوع القيمة
اطبع_سطر("نوع العمر: " + نوع(عمر))
اطبع_سطر("نوع الاسم: " + نوع(اسم))`,exercise:`# تمرين: عرّف متغيراً من كل نوع يصف كتاباً:
# عنوان (نص)، عدد الصفحات (رقم)، السعر (عشري)، متوفر (منطقي)
# ثم اطبع نوع كل متغير باستخدام نوع()
`,hint:"استخدم نوع(المتغير) لمعرفة نوعه",tips:["اللغة تستنتج الأنواع تلقائياً — لا حاجة لتحديدها","يمكنك تحديد النوع صراحة: رقم عمر = 25",'القيمة لاشيء تمثل "لا قيمة" (null)'],concepts:["رقم","عشري","نص","منطقي","نوع"]}]},Yl={id:"l1_s2",title:"الأرقام والحساب",icon:"🔢",lessons:[{id:"l1_09",title:"الأعداد الصحيحة",desc:"الأعداد الصحيحة (integers) بدون فاصلة عشرية",explanation:"الأعداد الصحيحة هي أبسط أنواع الأرقام وأكثرها استخداماً في البرمجة. هي أعداد بدون فاصلة عشرية: 1، 42، -7، 0، 1000000. في الحياة اليومية، نستخدمها لعدّ الأشياء (5 كتب)، وترتيبها (الطابق 3)، وتمثيل الكميات الكاملة (عمرك 25 سنة).\n\nفي لغة ص، نوع العدد الصحيح هو `رقم`. يمكن أن يكون العدد موجباً (25)، أو سالباً (-10)، أو صفراً (0). لا تحتاج لعلامة خاصة للأعداد الموجبة، لكن الأعداد السالبة تبدأ بعلامة الناقص `-`. يمكنك إجراء جميع العمليات الحسابية على الأعداد الصحيحة: الجمع والطرح والضرب والقسمة.\n\nعند قسمة عددين صحيحين، النتيجة قد تكون عدداً عشرياً. مثلاً `7 / 2` يعطي `3.5`. هذا سلوك طبيعي في لغة ص — اللغة تحافظ على الدقة تلقائياً. الدالة `مطلق()` تُرجع القيمة المطلقة لأي عدد، أي تحوّل السالب إلى موجب.\n\nالأعداد الصحيحة مثالية لتمثيل العدادات، والمؤشرات، والكميات، والأعمار، وأي شيء لا يحتاج إلى كسور. كلما استطعت استخدام عدد صحيح بدل عشري، يُفضّل ذلك لأنه أدق وأسرع في الحساب.",code:`# أعداد صحيحة موجبة
متغير عمر = 25
متغير عدد_طلاب = 1500

# أعداد صحيحة سالبة
متغير درجة = -10
متغير عمق = -200

# الصفر
متغير بداية = 0

# عمليات على الأعداد الصحيحة
اطبع_سطر("العمر + 5 = " + (عمر + 5))
اطبع_سطر("الدرجة × 2 = " + (درجة * 2))
اطبع_سطر("القيمة المطلقة: " + مطلق(درجة))`,exercise:`# تمرين: احسب عدد الثواني في يوم كامل
# تلميح: 24 ساعة × 60 دقيقة × 60 ثانية
`,hint:"متغير ثواني_اليوم = 24 * 60 * 60",tips:["الأعداد الصحيحة مثالية للعدّ والترتيب","يمكن أن تكون موجبة أو سالبة أو صفر"],concepts:["أعداد صحيحة","رقم"]},{id:"l1_10",title:"الأعداد العشرية",desc:"الأعداد العشرية (float) للقيم ذات الكسور",explanation:"الأعداد العشرية (تُسمى أيضاً أعداد الفاصلة العائمة أو float) هي أرقام تحتوي على جزء كسري بعد الفاصلة. نجدها في كل مكان: الأسعار (29.99 ريال)، الأطوال (1.75 متر)، درجات الحرارة (36.6°)، والنسب (95.5%).\n\nفي لغة ص، نوع العدد العشري هو `عشري`. لكتابة عدد عشري، استخدم النقطة `.` كفاصلة عشرية (وليس الفاصلة العربية). مثلاً: `متغير سعر = 29.99`. عند مزج عدد صحيح مع عشري في عملية حسابية، النتيجة تكون دائماً عشرية: `10 + 0.5` يعطي `10.5`.\n\nملاحظة مهمة: الأعداد العشرية في الحاسوب ليست دقيقة بنسبة 100% دائماً. هذا ليس عيباً في لغة ص بل في طريقة تمثيل الفاصلة العائمة في جميع لغات البرمجة. مثلاً، `0.1 + 0.2` قد لا يساوي `0.3` بالضبط. في أغلب الحالات هذا الفرق ضئيل جداً ولا يؤثر، لكن يجب الحذر عند مقارنة الأعداد العشرية.\n\nاستخدم الأعداد العشرية عندما تحتاج للدقة الكسرية: القياسات، الأسعار، النسب، والحسابات العلمية. وتذكّر أن `نوع()` ستُخبرك بنوع العدد إذا لم تكن متأكداً.",code:`# أعداد عشرية
متغير طول = 1.75
متغير وزن = 68.5
متغير حرارة = 36.6
متغير سالب = -3.14

# لاحظ الفرق — القسمة الصحيحة VS العشرية
متغير نتيجة1 = 7 / 2
اطبع_سطر("7 / 2 = " + نتيجة1)

# مزج الأنواع
متغير مجموع = 10 + 0.5
اطبع_سطر("10 + 0.5 = " + مجموع)
اطبع_سطر("نوع النتيجة: " + نوع(مجموع))`,exercise:`# تمرين: احسب مؤشر كتلة الجسم BMI
# BMI = الوزن (كغ) ÷ الطول (متر) تربيع
# مثال: وزن=70، طول=1.75
`,hint:"متغير bmi = الوزن / (الطول * الطول)",tips:["الأعداد العشرية تستخدم النقطة . وليس الفاصلة","عمليات الأعداد العشرية قد لا تكون دقيقة 100% بسبب التمثيل الثنائي"],concepts:["عشري","أعداد عشرية","float"]},{id:"l1_11",title:"العمليات الحسابية الأساسية",desc:"الجمع والطرح والضرب والقسمة — أساس الحساب",explanation:'العمليات الحسابية الأربع هي أساس كل الحسابات في البرمجة. تماماً مثل ما تعلمته في المدرسة: الجمع `+` والطرح `-` والضرب `*` والقسمة `/`. في لغة ص، هذه العوامل تعمل بشكل طبيعي على الأرقام.\n\nعامل الجمع `+` يجمع رقمين، وله استخدام إضافي مع النصوص حيث يدمج (يلصق) نصين معاً. عامل الطرح `-` يطرح الرقم الثاني من الأول. عامل الضرب `*` يضرب رقمين. عامل القسمة `/` يقسم الرقم الأول على الثاني — وتذكّر أن القسمة على صفر تسبب خطأ!\n\nعند كتابة تعبيرات حسابية داخل `اطبع_سطر()` مع نصوص، ضع التعبير الحسابي بين أقواس `()`. مثلاً: `اطبع_سطر("الناتج: " + (5 + 3))`. بدون الأقواس، العامل `+` سيدمج النص مع الرقم الأول فقط بدلاً من حساب المجموع.\n\nهذه العمليات الأربع كافية لبناء حسابات معقدة: من حساب الرواتب إلى تحويل العملات إلى حساب المساحات. وعند دمجها مع المتغيرات والثوابت، يمكنك كتابة برامج حسابية قوية ومفيدة.',code:`متغير أ = 20
متغير ب = 7

# العمليات الأربعة الأساسية
اطبع_سطر(أ + " + " + ب + " = " + (أ + ب))
اطبع_سطر(أ + " - " + ب + " = " + (أ - ب))
اطبع_سطر(أ + " × " + ب + " = " + (أ * ب))
اطبع_سطر(أ + " ÷ " + ب + " = " + (أ / ب))

# مثال عملي: حساب الراتب
ثابت الساعات = 160
ثابت أجر_الساعة = 50
متغير الراتب = الساعات * أجر_الساعة
اطبع_سطر("الراتب الشهري: " + الراتب + " ريال")`,exercise:`# تمرين: اكتب حاسبة مساحة مستطيل
# المساحة = الطول × العرض
# المحيط = 2 × (الطول + العرض)
`,hint:"عرّف الطول والعرض كمتغيرات ثم احسب",tips:["الأقواس () ضرورية لتحديد ترتيب العمليات في اطبع_سطر","الضرب * والقسمة / أولوية أعلى من + و -"],concepts:["+","-","*","/","عمليات حسابية"]},{id:"l1_12",title:"الأس وباقي القسمة",desc:"عاملا الأس ** وباقي القسمة % — أدوات رياضية مهمة",explanation:'بالإضافة إلى العمليات الأربع الأساسية، لغة ص توفر عاملين مهمين جداً: الأس `**` وباقي القسمة `%`. هذان العاملان يفتحان باباً واسعاً من الحسابات الرياضية والمنطقية.\n\nعامل الأس `**` يرفع عدداً إلى قوة معينة. مثلاً: `2 ** 3` تعني "2 أس 3" أي 2 × 2 × 2 = 8. هذا أسهل وأوضح من كتابة `2 * 2 * 2`، خاصة مع الأسس الكبيرة. أي عدد أس 0 يساوي 1، وأي عدد أس 1 يساوي نفسه.\n\nعامل باقي القسمة `%` يُرجع الباقي بعد القسمة. مثلاً: `17 % 5` تساوي 2 لأن 17 ÷ 5 = 3 والباقي 2. هذا العامل مفيد جداً لمعرفة هل عدد زوجي أم فردي: إذا كان `عدد % 2 == 0` فهو زوجي. كذلك يمكن استخدامه لمعرفة هل عدد قابل للقسمة على عدد آخر.\n\nاستخدامات عملية: `**` يُستخدم في الحسابات الهندسية (المساحات والحجوم) وفي الخوارزميات الرياضية. `%` يُستخدم في أنماط التكرار، وتحديد الأيام، وتوزيع العناصر بالتساوي، والعديد من المسائل البرمجية الشائعة.',code:`# الأس ** (القوة / Power)
اطبع_سطر("2 أس 3 = " + (2 ** 3))      # 8
اطبع_سطر("5 أس 2 = " + (5 ** 2))      # 25
اطبع_سطر("10 أس 0 = " + (10 ** 0))    # 1

# باقي القسمة % (Modulo)
اطبع_سطر("17 % 5 = " + (17 % 5))      # 2
اطبع_سطر("20 % 4 = " + (20 % 4))      # 0
اطبع_سطر("7 % 2 = " + (7 % 2))        # 1

# استخدام عملي: هل العدد زوجي؟
متغير عدد = 42
إذا (عدد % 2 == 0)
    اطبع_سطر(عدد + " عدد زوجي")
وإلا
    اطبع_سطر(عدد + " عدد فردي")
نهاية`,exercise:`# تمرين: احسب:
# 1. مربع العدد 12
# 2. مكعب العدد 5
# 3. هل العدد 99 قابل للقسمة على 3؟
`,hint:"المربع = عدد ** 2 والمكعب = عدد ** 3 والقسمة على 3: عدد % 3 == 0",tips:["% يُرجع الباقي — مفيد لمعرفة الزوجية والقسمة","** يُستخدم بدل كتابة x * x * x ..."],concepts:["**","%","أس","باقي القسمة"]},{id:"l1_13",title:"أولوية العمليات",desc:"من يُحسب أولاً؟ ترتيب العمليات الحسابية",explanation:'عندما يحتوي تعبير رياضي على عدة عوامل، يجب أن يعرف الحاسوب أيها يُحسب أولاً. هذا ما نسميه "أولوية العمليات" — وهي نفس القواعد التي تعلمتها في الرياضيات المدرسية. مثلاً: `2 + 3 * 4` يساوي 14 وليس 20، لأن الضرب يُحسب قبل الجمع.\n\nترتيب الأولويات في لغة ص (من الأعلى للأدنى): الأقواس `()` أولاً — دائماً تُحسب أولاً. ثم الأس `**`. ثم الضرب `*` والقسمة `/` وباقي القسمة `%` (نفس الأولوية). وأخيراً الجمع `+` والطرح `-`. عندما تتساوى الأولوية، العمليات تُحسب من اليسار إلى اليمين.\n\nنصيحة ذهبية: عند الشك في ترتيب العمليات، استخدم الأقواس! الأقواس لا تغير فقط ترتيب الحساب، بل تجعل الكود أسهل للقراءة. حتى لو كنت متأكداً من الترتيب، إضافة أقواس توضيحية تساعد من يقرأ كودك لاحقاً.\n\nفهم أولوية العمليات ضروري لكتابة حسابات صحيحة. خطأ في الترتيب قد يعطي نتائج خاطئة تماماً — مثل حساب ضريبة أو خصم بشكل غير صحيح. دائماً تحقق من نتائجك وأضف أقواساً عندما يكون التعبير معقداً.',code:`# الأولويات (من الأعلى للأدنى):
# 1. الأقواس ()
# 2. الأس **
# 3. الضرب * والقسمة / والباقي %
# 4. الجمع + والطرح -

اطبع_سطر("2 + 3 * 4 = " + (2 + 3 * 4))        # 14 (وليس 20)
اطبع_سطر("(2 + 3) * 4 = " + ((2 + 3) * 4))    # 20

اطبع_سطر("2 ** 3 + 1 = " + (2 ** 3 + 1))      # 9
اطبع_سطر("2 ** (3 + 1) = " + (2 ** (3 + 1)))   # 16

# مثال عملي: حساب السعر بعد الخصم والضريبة
ثابت السعر = 100
ثابت الخصم = 20    # 20%
ثابت الضريبة = 15  # 15%
متغير بعد_الخصم = السعر * (1 - الخصم / 100)
متغير النهائي = بعد_الخصم * (1 + الضريبة / 100)
اطبع_سطر("السعر النهائي: " + النهائي)`,exercise:`# تمرين: ما نتيجة كل تعبير؟ (خمّن أولاً ثم تحقق)
# 1. 10 - 2 * 3
# 2. (10 - 2) * 3
# 3. 2 + 3 ** 2
# 4. 100 / 10 / 2
`,hint:"تذكر: ** ثم */ ثم +-",tips:["عند الشك، استخدم الأقواس لتكون واضحاً","الأقواس تجعل الكود أسهل قراءة حتى لو لم تكن ضرورية"],concepts:["أولوية العمليات","أقواس"]},{id:"l1_14",title:"عوامل الإسناد المركبة",desc:"اختصارات لتعديل قيمة المتغير: += -= *= /= %=",explanation:"عوامل الإسناد المركبة هي اختصارات ذكية لتعديل قيمة متغير بناءً على قيمته الحالية. بدلاً من كتابة `نقاط = نقاط + 10`، يمكنك ببساطة كتابة `نقاط += 10`. الاختصار يفعل نفس الشيء بالضبط لكنه أقصر وأوضح.\n\nلغة ص توفر خمسة عوامل إسناد مركبة: `+=` للجمع، `-=` للطرح، `*=` للضرب، `/=` للقسمة، و`%=` لباقي القسمة. كل عامل يأخذ القيمة الحالية للمتغير، يُجري العملية عليها مع القيمة الجديدة، ثم يُخزّن النتيجة في نفس المتغير.\n\nالاستخدام الأكثر شيوعاً هو العدّاد: `عداد += 1` يزيد العداد بواحد في كل مرة. هذا النمط أساسي في الحلقات والتكرار الذي سنتعلمه لاحقاً. كذلك يُستخدم `-=` كثيراً في أنظمة النقاط والأرصدة حيث تحتاج لخصم قيم.\n\nهذه الاختصارات ليست مجرد كتابة أقل — بل تجعل النية أوضح. عندما يقرأ شخص `رصيد += 500`، يفهم فوراً أننا نُضيف إلى الرصيد الحالي، بينما `رصيد = رصيد + 500` تحتاج لقراءة أكثر لفهم العملية.",code:`متغير نقاط = 100

# بدل كتابة: نقاط = نقاط + 10
نقاط += 10
اطبع_سطر("بعد +10: " + نقاط)    # 110

نقاط -= 30
اطبع_سطر("بعد -30: " + نقاط)    # 80

نقاط *= 2
اطبع_سطر("بعد ×2: " + نقاط)     # 160

نقاط /= 4
اطبع_سطر("بعد ÷4: " + نقاط)     # 40

نقاط %= 7
اطبع_سطر("بعد %7: " + نقاط)     # 5

# مثال عملي: عداد
متغير عداد = 0
عداد += 1     # زيادة بواحد
عداد += 1
عداد += 1
اطبع_سطر("العداد: " + عداد)     # 3`,exercise:`# تمرين: لديك رصيد = 1000
# 1. أضف 500 (إيداع)
# 2. اخصم 200 (سحب)
# 3. ضاعف الرصيد (× 2)
# 4. اقسم على 3
# اطبع الرصيد بعد كل عملية
`,hint:"استخدم += و -= و *= و /=",tips:["x += 5 هو اختصار لـ x = x + 5","هذه الاختصارات تجعل الكود أقصر وأوضح"],concepts:["+=","-=","*=","/=","%="]},{id:"l1_15",title:"تحويل الأنواع",desc:"حوّل بين الأرقام والنصوص والأنواع الأخرى",explanation:'في كثير من الأحيان، تحتاج لتحويل قيمة من نوع إلى آخر. مثلاً، عندما تقرأ إدخال المستخدم بـ `اقرأ()`، تحصل دائماً على نص — حتى لو كتب المستخدم رقماً. لجمع رقمين من المستخدم، يجب أولاً تحويل النص إلى رقم.\n\nلغة ص توفر ثلاث دوال أساسية للتحويل: `لرقم()` تحوّل نصاً إلى عدد صحيح (مثلاً `لرقم("42")` تُنتج 42). `لعشري()` تحوّل نصاً إلى عدد عشري (مثلاً `لعشري("3.14")` تُنتج 3.14). `لنص()` تحوّل أي قيمة إلى نص (مثلاً `لنص(100)` تُنتج "100").\n\nالنمط الشائع في البرامج التفاعلية هو: اقرأ الإدخال كنص → حوّله للنوع المطلوب → استخدمه في الحسابات. مثلاً: `متغير العمر = لرقم(اقرأ())` تقرأ نصاً من المستخدم وتحوّله مباشرة إلى رقم. احذر: إذا أدخل المستخدم نصاً لا يمكن تحويله (مثل "مرحبا" إلى رقم)، سيحدث خطأ.\n\nالدالة `نوع()` هي أداة تشخيصية مفيدة — تُرجع اسم نوع أي قيمة. استخدمها عندما لا تكون متأكداً من نوع متغير ما. فهم التحويل بين الأنواع أساسي لكتابة برامج خالية من الأخطاء.',code:`# من نص إلى رقم
متغير نص_رقمي = "42"
متغير رقم = لرقم(نص_رقمي)
اطبع_سطر(رقم + 8)    # 50 (وليس "428")

# من رقم إلى نص
متغير عدد = 100
متغير نص = لنص(عدد)
اطبع_سطر("العدد هو " + نص)

# من نص إلى عشري
متغير سعر_نص = "29.99"
متغير سعر = لعشري(سعر_نص)
اطبع_سطر("السعر: " + سعر)

# معرفة النوع
اطبع_سطر(نوع(42))         # رقم
اطبع_سطر(نوع(3.14))       # عشري
اطبع_سطر(نوع("نص"))       # نص
اطبع_سطر(نوع(صحيح))       # منطقي`,exercise:`# تمرين: اطلب من المستخدم رقمين (كنص)
# حوّلهما لأرقام واجمعهما
# اطبع النتيجة
`,hint:"اقرأ() ترجع نصاً — حوّله بـ لرقم()",tips:["اقرأ() تُرجع دائماً نصاً حتى لو أدخل المستخدم رقماً","لرقم() تحول نصاً لعدد صحيح، لعشري() لعدد عشري","نوع(قيمة) تُخبرك بنوع أي قيمة"],concepts:["لرقم","لنص","لعشري","نوع","تحويل الأنواع"]},{id:"l1_16",title:"الدوال الرياضية المدمجة",desc:"استخدم دوال الرياضيات الجاهزة",explanation:'لغة ص توفر مجموعة من الدوال الرياضية الجاهزة التي يمكنك استخدامها مباشرة بدون أي إعدادات أو استيراد. هذه الدوال تُغنيك عن كتابة حسابات معقدة بنفسك وتجعل كودك أبسط وأقل عرضة للأخطاء.\n\nأهم الدوال المدمجة: `مطلق()` تُرجع القيمة المطلقة (تحوّل السالب إلى موجب). `أكبر()` تُرجع الأكبر بين قيمتين. `أصغر()` تُرجع الأصغر. `جذر()` تحسب الجذر التربيعي. `مجموع()` تجمع كل عناصر مصفوفة. هذه الدوال تقبل أرقاماً صحيحة وعشرية.\n\nالدوال الرياضية أساسية في التطبيقات العملية. مثلاً، حساب المسافة بين نقطتين يستخدم `جذر()` و`**`: المسافة = جذر((س2 - س1)² + (ص2 - ص1)²). حساب الوتر في مثلث قائم يستخدم نفس الصيغة. إيجاد أعلى درجة بين الطلاب يستخدم `أكبر()`.\n\nهذه فقط البداية — لغة ص توفر دوالاً رياضية أكثر في المكتبة القياسية سنتعرف عليها لاحقاً. المفهوم المهم هنا هو أن "الدالة" هي كتلة كود جاهزة تقوم بمهمة محددة وتُرجع نتيجة — وسنتعلم كيف نكتب دوالنا الخاصة في الدروس القادمة.',code:`# القيمة المطلقة
اطبع_سطر("مطلق(-15) = " + مطلق(-15))     # 15

# الأكبر والأصغر
اطبع_سطر("أكبر(10, 20) = " + أكبر(10, 20)) # 20
اطبع_سطر("أصغر(10, 20) = " + أصغر(10, 20)) # 10

# الجذر التربيعي
اطبع_سطر("جذر(144) = " + جذر(144))       # 12

# مجموع مصفوفة
متغير أرقام = [10, 20, 30, 40]
اطبع_سطر("المجموع = " + مجموع(أرقام))   # 100

# مثال عملي: حساب المسافة بين نقطتين
متغير س1 = 0
متغير ص1 = 0
متغير س2 = 3
متغير ص2 = 4
متغير مسافة = جذر((س2 - س1) ** 2 + (ص2 - ص1) ** 2)
اطبع_سطر("المسافة = " + مسافة)    # 5`,exercise:`# تمرين: أوجد الوتر في مثلث قائم الزاوية
# الضلع الأول = 5, الضلع الثاني = 12
# الوتر = جذر(أ² + ب²)
`,hint:"استخدم جذر(أ ** 2 + ب ** 2)",tips:["هذه الدوال جاهزة — لا حاجة لاستيرادها","يمكنك كتابة sqrt بدل جذر و abs بدل مطلق"],concepts:["مطلق","أكبر","أصغر","جذر","مجموع"]}]},Xl={id:"l1_s3",title:"النصوص",icon:"📝",lessons:[{id:"l1_17",title:"تعريف النصوص واستخدامها",desc:"النصوص (strings) سلاسل من الحروف تُحاط بعلامات تنصيص",explanation:`النصوص هي أحد أهم أنواع البيانات في البرمجة. تخيل أن النص هو سلسلة من الخرز — كل خرزة هي حرف. عندما نكتب "مرحبا"، فإننا نضع 5 حروف (خرزات) في سلسلة واحدة. في لغة ص، نُحيط النص بعلامات تنصيص مزدوجة "..." لنخبر الحاسوب أن هذا نص وليس أمر برمجي.

أحياناً نحتاج وضع رموز خاصة داخل النص مثل سطر جديد أو علامة تبويب. هنا نستخدم حروف الهروب (escape sequences): \\n للانتقال لسطر جديد، و\\t لإضافة مسافة تبويب. إذا أردت وضع علامة تنصيص داخل النص نفسه، اكتب \\" حتى لا يظن الحاسوب أنك أنهيت النص.

النص الفارغ "" هو نص موجود لكنه لا يحتوي على أي حروف — طوله صفر. هذا مفيد عندما نريد بناء نص تدريجياً بإضافة أجزاء إليه لاحقاً. لاحظ أن لغة ص تدعم يونيكود (Unicode) بالكامل، مما يعني أنك تستطيع كتابة النصوص بالعربية والإنجليزية وأي لغة أخرى بسهولة.`,code:`# نصوص بعلامات تنصيص مزدوجة
متغير تحية = "مرحبا بالعالم!"
متغير اسم = "أحمد"

# حروف خاصة (escape sequences)
متغير سطر_جديد = "سطر أول\\nسطر ثاني"
متغير تبويب = "عمود1\\tعمود2"
متغير تنصيص = "قال: \\"مرحبا\\""

اطبع_سطر(تحية)
اطبع_سطر(سطر_جديد)
اطبع_سطر(تبويب)
اطبع_سطر(تنصيص)

# النص الفارغ
متغير فارغ = ""
اطبع_سطر("طول الفارغ: " + طول(فارغ))`,exercise:`# تمرين: اطبع قصيدة قصيرة على عدة أسطر
# استخدم \\n للأسطر الجديدة
`,hint:"استخدم \\n داخل النص للانتقال لسطر جديد",tips:["\\n سطر جديد، \\t تبويب، \\\\ شرطة مائلة عكسية","النصوص في لغة ص تدعم Unicode كاملاً"],concepts:["نص","حروف هروب","\\n","\\t"]},{id:"l1_18",title:"دمج النصوص (Concatenation)",desc:"اربط نصوصاً متعددة معاً بعامل الجمع +",explanation:'دمج النصوص يشبه لصق أوراق ملاحظات ببعضها — تأخذ ورقة مكتوب عليها "مرحبا" وورقة أخرى مكتوب عليها " يا أحمد" وتلصقهما معاً لتحصل على "مرحبا يا أحمد". في لغة ص، نستخدم عامل الجمع `+` لدمج نصين أو أكثر.\n\nيمكنك أيضاً دمج نص مع رقم مباشرة، واللغة ستحوّل الرقم إلى نص تلقائياً. مثلاً: `"عمري " + 25 + " سنة"` يُنتج النص "عمري 25 سنة". هذا مفيد جداً عند بناء رسائل تحتوي على قيم متغيرة.\n\nالعامل `+=` يُضيف نصاً لمتغير نصي موجود. فبدلاً من كتابة `رسالة = رسالة + "نص جديد"`، يمكنك اختصارها إلى `رسالة += "نص جديد"`. هذا مفيد جداً عند بناء نصوص طويلة خطوة بخطوة، كبناء رسالة بريد إلكتروني أو تقرير.',code:`متغير الاسم_الأول = "محمد"
متغير الاسم_الأخير = "أحمد"

# دمج نصوص
متغير الاسم_الكامل = الاسم_الأول + " " + الاسم_الأخير
اطبع_سطر(الاسم_الكامل)

# دمج نص مع أرقام
متغير عمر = 25
اطبع_سطر("عمري " + عمر + " سنة")

# بناء رسالة طويلة
متغير رسالة = "مرحبا يا " + الاسم_الأول + "!"
رسالة += "\\n"
رسالة += "عمرك " + عمر + " سنة."
اطبع_سطر(رسالة)`,exercise:`# تمرين: ابنِ بطاقة تعريف من متغيرات منفصلة
# الاسم، الجنسية، العمر، المهنة
# النتيجة: "أنا [الاسم]، [مهنة] [جنسية]، عمري [عمر] سنة"
`,hint:"ادمج المتغيرات مع نصوص ثابتة باستخدام +",tips:["+ يدمج نصوصاً ويمكنه تحويل الأرقام تلقائياً","يمكن استخدام += لإضافة نص لمتغير موجود"],concepts:["دمج النصوص","+","+="]},{id:"l1_19",title:"طول النص والوصول للحروف",desc:"احسب طول النص والوصول لحروف بالفهرس",explanation:'كل نص في البرمجة له طول — أي عدد الحروف التي يحتويها. الدالة `طول()` تُرجع هذا العدد. مثلاً، `طول("مرحبا")` يُرجع 5. معرفة الطول ضرورية في كثير من المواقف، مثل التحقق من أن كلمة السر طويلة كفاية.\n\nكل حرف في النص له رقم (فهرس) يبدأ من الصفر وليس من الواحد. فكلمة "مرحبا": الحرف م فهرسه 0، ر فهرسه 1، ح فهرسه 2، وهكذا. للوصول لحرف معين نستخدم الأقواس المربعة: `كلمة[0]` تعطيك الحرف الأول. هذا المفهوم (الفهرسة من الصفر) أساسي جداً في كل لغات البرمجة.\n\nيمكنك أيضاً التكرار على حروف النص باستخدام حلقة `لكل`. هذا يسمح لك بفحص كل حرف على حدة، مثلاً لعدّ حروف معينة أو البحث عن نمط. ستتعلم المزيد عن الحلقات في الدروس القادمة.',code:`متغير كلمة = "مرحبا"

# طول النص
اطبع_سطر("الطول: " + طول(كلمة))  # 5

# الوصول لحرف بالفهرس (يبدأ من 0)
اطبع_سطر("الحرف الأول: " + كلمة[0])   # م
اطبع_سطر("الحرف الثاني: " + كلمة[1])  # ر
اطبع_سطر("الحرف الأخير: " + كلمة[4])  # ا

# التكرار على حروف النص
لكل حرف في كلمة
    اطبع(حرف + " ")
نهاية
اطبع_سطر("")`,exercise:`# تمرين: اطبع حروف اسمك مع أرقامها
# مثال: 0:م 1:ح 2:م 3:د
`,hint:"استخدم حلقة لكل مع فهرس",tips:["الفهرسة تبدأ من 0 وليس 1",'طول("مرحبا") = 5 لكن آخر فهرس هو 4'],concepts:["طول","فهرسة","النص[i]"]},{id:"l1_20",title:"البحث في النصوص",desc:"ابحث عن كلمة أو حرف داخل نص",explanation:'البحث في النصوص يشبه البحث عن كلمة في كتاب. في لغة ص، لديك عدة أدوات للبحث. الدالة `يحتوي_على()` تُخبرك ببساطة: هل هذه الكلمة موجودة في النص أم لا؟ تُرجع `صحيح` أو `خطأ`.\n\nالدالتان `يبدأ_ب()` و`ينتهي_ب()` أكثر تحديداً — تفحصان بداية أو نهاية النص فقط. مثلاً، `يبدأ_ب("مرحبا يا أحمد", "مرحبا")` ترجع `صحيح`. هذه مفيدة للتحقق من أنماط معينة، كالتأكد من أن رابطاً يبدأ بـ "https://".\n\nالدالة `بحث()` تذهب أبعد — لا تخبرك فقط بوجود الكلمة، بل تُعطيك موقعها (رقم الحرف الذي تبدأ عنده). إذا لم تجد الكلمة، تُرجع -1. هذا مفيد عندما تحتاج لمعرفة أين بالضبط تقع الكلمة في النص لتتمكن من استخراجها أو تعديلها.',code:`متغير جملة = "أهلاً بك في عالم البرمجة"

# التحقق من وجود كلمة
اطبع_سطر(يحتوي_على(جملة, "برمجة"))  # صحيح
اطبع_سطر(يحتوي_على(جملة, "لغة ص"))  # خطأ

# يبدأ بـ / ينتهي بـ
اطبع_سطر(يبدأ_ب(جملة, "أهلاً"))   # صحيح
اطبع_سطر(ينتهي_ب(جملة, "برمجة"))  # صحيح

# إيجاد موقع كلمة
متغير موقع = بحث(جملة, "عالم")
اطبع_سطر("موقع 'عالم': " + موقع)`,exercise:`# تمرين: اسأل المستخدم عن جملة وكلمة للبحث عنها
# ثم اطبع هل الكلمة موجودة أم لا
`,hint:"استخدم يحتوي_على(الجملة, الكلمة)",tips:["يحتوي_على تُرجع صحيح أو خطأ","بحث تُرجع رقم الموقع أو -1 إذا لم يُوجد"],concepts:["يحتوي_على","يبدأ_ب","ينتهي_ب","بحث"]},{id:"l1_21",title:"استخراج أجزاء من النص",desc:"اقتطع جزءاً من نص باستخدام المواضع",explanation:'أحياناً لا تحتاج النص كاملاً، بل تريد جزءاً منه فقط — مثل قص قطعة من شريط. دالة `استخراج(نص, بداية, طول)` تقتطع جزءاً من النص بدءاً من موضع معين. مثلاً، `استخراج("مرحبا بالعالم", 0, 5)` تعطيك "مرحبا".\n\nدالة `قسم()` تقسم النص إلى أجزاء عند فاصل معين وتُرجع مصفوفة. مثلاً، `قسم("أحمد,محمد,سارة", ",")` تعطيك مصفوفة من ثلاثة أسماء. هذه الدالة مفيدة جداً في معالجة البيانات، كقراءة ملفات CSV أو تحليل إدخال المستخدم.\n\nالعكس هو دالة `صل()` التي تدمج مصفوفة نصوص في نص واحد مع فاصل بينها. مثلاً، `صل(["لغة", "ص", "رائعة"], " ")` تعطيك "لغة ص رائعة". هاتان الدالتان (`قسم` و`صل`) متكاملتان وستستخدمهما كثيراً في مشاريعك.',code:`متغير نص = "مرحبا بالعالم العربي"

# استخراج جزء: استخراج(نص, بداية, نهاية)
متغير أول_كلمة = استخراج(نص, 0, 5)
اطبع_سطر("أول كلمة: " + أول_كلمة)  # مرحبا

# تقسيم النص بفاصل
متغير أجزاء = قسم(نص, " ")
لكل جزء في أجزاء
    اطبع_سطر("→ " + جزء)
نهاية

# دمج مصفوفة نصوص
متغير كلمات = ["لغة", "ص", "رائعة"]
متغير جملة = صل(كلمات, " ")
اطبع_سطر(جملة)  # لغة ص رائعة`,exercise:`# تمرين: لديك نص بريد إلكتروني "user@example.com"
# استخرج اسم المستخدم والنطاق (domain)
`,hint:'قسم البريد بالفاصل "@" ثم اطبع كل جزء',tips:["استخراج(نص, بداية, طول) تقتطع جزءاً","قسم(نص, فاصل) تقسم النص لمصفوفة","صل(مصفوفة, فاصل) تدمج مصفوفة في نص واحد"],concepts:["استخراج","قسم","صل","substring","split","join"]},{id:"l1_22",title:"تحويل حالة الأحرف",desc:"حوّل بين الأحرف الكبيرة والصغيرة",explanation:'الأحرف اللاتينية لها شكلان: كبير (A, B, C) وصغير (a, b, c). دالة `أحرف_كبيرة()` تحوّل كل الأحرف الصغيرة إلى كبيرة، و`أحرف_صغيرة()` تفعل العكس. هذه مفيدة عند مقارنة النصوص بدون اعتبار لحالة الأحرف.\n\nدالة `قص_أطراف()` تزيل المسافات الزائدة من بداية ونهاية النص. تخيل أن المستخدم كتب "  أحمد  " — هذه الدالة تحوّلها إلى "أحمد" نظيفة. مفيدة جداً عند التعامل مع إدخال المستخدم الذي قد يحتوي مسافات غير مقصودة.\n\nدالة `استبدل()` تستبدل كلمة أو عبارة بأخرى داخل النص. مثلاً، يمكنك استبدال كل كلمة "مرحبا" بكلمة "أهلاً" في رسالة. لاحظ أن العربية ليس لها مفهوم الأحرف الكبيرة والصغيرة، لذا دوال التحويل تؤثر فقط على الأحرف اللاتينية في النص.',code:`متغير نص = "Hello World"

# تحويل للأحرف الكبيرة
اطبع_سطر(أحرف_كبيرة(نص))  # HELLO WORLD

# تحويل للأحرف الصغيرة
اطبع_سطر(أحرف_صغيرة(نص))  # hello world

# إزالة المسافات من الأطراف
متغير مع_مسافات = "  مرحبا  "
اطبع_سطر("'" + قص_أطراف(مع_مسافات) + "'")  # 'مرحبا'

# استبدال
متغير جملة = "أحب البرمجة بلغة Python"
متغير جديد = استبدل(جملة, "Python", "ص")
اطبع_سطر(جديد)  # أحب البرمجة بلغة ص`,exercise:`# تمرين: اطلب اسم المستخدم ثم:
# 1. اطبعه بأحرف كبيرة
# 2. اطبعه بأحرف صغيرة
# 3. اطبع طوله
`,hint:"استخدم أحرف_كبيرة() و أحرف_صغيرة() و طول()",tips:["العربية ليس لها أحرف كبيرة/صغيرة — هذه الدوال تؤثر على الأحرف اللاتينية","قص_أطراف() مفيدة لتنظيف إدخال المستخدم"],concepts:["أحرف_كبيرة","أحرف_صغيرة","قص_أطراف","استبدل"]},{id:"l1_23",title:"النصوص المُنسقة (F-Strings)",desc:"ادمج المتغيرات داخل النصوص بطريقة أنيقة",explanation:'النصوص المنسقة (f-strings) هي طريقة أنيقة وسهلة لدمج المتغيرات داخل النصوص. بدلاً من استخدام `+` لدمج الأجزاء، ببساطة ضع حرف `f` قبل علامة التنصيص ثم اكتب اسم المتغير بين قوسين معقوفين `{}` مباشرة في النص.\n\nالميزة الرائعة في f-string أنك تستطيع وضع أي تعبير حسابي أو منطقي داخل `{}` وليس فقط أسماء المتغيرات. مثلاً: `f"سنة الميلاد: {2026 - عمر}"` يحسب الطرح مباشرة ويضع النتيجة في النص. يمكنك حتى استدعاء دوال داخل الأقواس المعقوفة.\n\nf-strings أسرع في التنفيذ وأسهل في القراءة من دمج النصوص التقليدي بـ `+`. تخيل أنك تكتب رسالة وتترك فراغات {} يملأها الحاسوب تلقائياً بقيم المتغيرات. إذا أردت طباعة القوس المعقوف نفسه { اكتب {{ مضاعفاً.',code:`متغير اسم = "أحمد"
متغير عمر = 25
متغير راتب = 5000.50

# بدل دمج النصوص التقليدي:
اطبع_سطر("مرحبا " + اسم + "، عمرك " + عمر)

# استخدم F-String — أسهل وأوضح!
اطبع_سطر(f"مرحبا {اسم}، عمرك {عمر} سنة")

# يمكنك وضع تعبيرات داخل {}
اطبع_سطر(f"سنة الميلاد: {2026 - عمر}")
اطبع_سطر(f"الراتب المضاعف: {راتب * 2}")
اطبع_سطر(f"هل بالغ؟ {عمر >= 18}")`,exercise:`# تمرين: اطبع فاتورة باستخدام f-string:
# "المنتج: [اسم] | الكمية: [عدد] | الإجمالي: [سعر×كمية]"
`,hint:"ضع f قبل النص ثم {تعبير} داخله",tips:["f-strings أسرع وأوضح من دمج النصوص العادي","يمكنك وضع أي تعبير داخل {} حتى استدعاء دوال","لطباعة { حرفياً اكتب {{"],concepts:["f-string","نص منسق","قوالب نصوص"]},{id:"l1_24",title:"النصوص الخام (Raw Strings)",desc:"نصوص تتجاهل الحروف الخاصة — مفيدة للمسارات",explanation:'النصوص الخام (raw strings) تتجاهل حروف الهروب تماماً. عادة عندما يرى الحاسوب \\n في نص يحوّله إلى سطر جديد، لكن في النص الخام يبقى \\n كما هو — حرفان عاديان. هذا مفيد عندما تريد أن يظهر النص كما كتبته بالضبط.\n\nلإنشاء نص خام ضع حرف `r` قبل علامة التنصيص: `r"النص هنا"`. هذا مفيد جداً عند كتابة مسارات الملفات على ويندوز التي تحتوي على شرطات مائلة عكسية \\. بدلاً من كتابة "C:\\\\مجلد\\\\ملف" (مضاعفة كل \\)، اكتب ببساطة `r"C:\\مجلد\\ملف"`.\n\nالنصوص الخام مفيدة أيضاً عند كتابة التعبيرات النمطية (regex) التي تستخدم \\ بكثرة. بدون r، ستحتاج لمضاعفة كل شرطة عكسية مما يجعل التعبير صعب القراءة. استخدم النص الخام `r"..."` كلما كان نصك يحتوي على \\ كثيرة.',code:`# نص عادي — يعالج حروف الهروب
متغير عادي = "مسافة\\tجديد\\nسطر"
اطبع_سطر(عادي)

# نص خام — لا يعالج حروف الهروب
متغير خام = r"مسافة\\tجديد\\nسطر"
اطبع_سطر(خام)  # يطبع \\t و \\n حرفياً

# مفيد لمسارات الملفات
متغير مسار = r"C:\\مجلد\\ملف.txt"
اطبع_سطر("المسار: " + مسار)

# مقارنة
اطبع_سطر("عادي: مرحبا\\nعالم")
اطبع_سطر(r"خام: مرحبا\\nعالم")`,exercise:`# تمرين: اطبع مسار ملف على ويندوز
# مثل: C:\\Users\\Ahmed\\Documents\\ملف.txt
# جرّب بنص عادي ثم بنص خام
`,hint:'مع r"..." لا حاجة لمضاعفة \\',tips:['r"..." يتجاهل كل حروف الهروب مثل \\n و \\t',"مفيد جداً لمسارات الملفات والتعبيرات النمطية",'Regex مثال: r"\\d+\\.\\d+"'],concepts:['r""',"نص خام","raw string"]}]},Zl={id:"l1_s4",title:"الشروط والمنطق",icon:"🔀",lessons:[{id:"l1_25",title:"جملة إذا البسيطة",desc:"اتخذ قراراً بناءً على شرط — أساس البرمجة",explanation:'الشروط هي أساس اتخاذ القرارات في البرمجة. تماماً كما في الحياة اليومية: "إذا كان الجو ماطراً، خذ المظلة". في لغة ص، نستخدم كلمة `إذا` متبوعة بشرط بين قوسين، ثم نكتب الكود الذي يُنفَّذ عند تحقق الشرط، ونختم بكلمة `نهاية`.\n\nالكود الذي بين `إذا` و`نهاية` يُنفَّذ فقط عندما يكون الشرط صحيحاً. إذا كان الشرط خاطئاً، يتجاهل الحاسوب هذا الكود بالكامل وينتقل لما بعد `نهاية`. يمكنك وضع عدة أسطر بين إذا ونهاية — كلها ستُنفَّذ أو تُتجاهل معاً.\n\nالأقواس حول الشرط اختيارية في لغة ص لكنها موصى بها لوضوح الكود. لا تنسَ كلمة `نهاية` — بدونها لن يعرف الحاسوب أين تنتهي الكتلة الشرطية. هذا المفهوم (تنفيذ كود بناءً على شرط) هو حجر الأساس الذي تُبنى عليه كل البرامج.',code:`متغير درجة_حرارة = 35

إذا (درجة_حرارة > 30)
    اطبع_سطر("الجو حار! 🌞")
نهاية

# مثال آخر
متغير عمر = 20

إذا (عمر >= 18)
    اطبع_سطر("أنت بالغ ✓")
    اطبع_سطر("يمكنك التصويت")
نهاية

اطبع_سطر("البرنامج انتهى")`,exercise:`# تمرين: اسأل المستخدم عن درجته
# إذا كانت >= 60 اطبع "ناجح!"
`,hint:"إذا (الدرجة >= 60) ... نهاية",tips:["الأقواس حول الشرط اختيارية لكن موصى بها","كل ما بين إذا و نهاية يُنفَّذ فقط إذا تحقق الشرط","لا تنسَ كلمة نهاية!"],concepts:["إذا","نهاية","شرط"]},{id:"l1_26",title:"إذا / وإلا — مساران",desc:"نفّذ كوداً بديلاً عندما لا يتحقق الشرط",explanation:'أحياناً لا يكفي أن تفعل شيئاً "إذا" تحقق الشرط — تريد أيضاً فعل شيء آخر "إذا لم" يتحقق. هنا تأتي كلمة `وإلا`. تخيل مفترق طرق: طريق يميناً إذا الضوء أخضر، ويساراً إذا أحمر. لا يمكنك السير في الطريقين معاً.\n\nالكود بين `إذا` و`وإلا` يُنفَّذ عند تحقق الشرط. الكود بين `وإلا` و`نهاية` يُنفَّذ عند عدم تحققه. واحد منهما فقط يُنفَّذ دائماً — لن يُنفَّذ كلاهما ولن يُتجاهل كلاهما. هذا يضمن أن برنامجك يتعامل مع كل الحالات.\n\n`وإلا` اختيارية. إذا كنت تريد فعل شيء عند تحقق الشرط فقط ولا شيء عند عدم تحققه، استخدم `إذا` بدون `وإلا`. لكن عندما تحتاج تغطية الحالتين (نجاح وفشل، مثلاً)، فـ `وإلا` هي أداتك.',code:`متغير رقم = 7

إذا (رقم % 2 == 0)
    اطبع_سطر(رقم + " عدد زوجي")
وإلا
    اطبع_سطر(رقم + " عدد فردي")
نهاية

# مثال عملي: التحقق من كلمة السر
متغير كلمة_السر = "1234"

إذا (كلمة_السر == "admin123")
    اطبع_سطر("مرحبا بك! 🔓")
وإلا
    اطبع_سطر("كلمة سر خاطئة! 🔒")
نهاية`,exercise:`# تمرين: اطلب من المستخدم رقماً
# إذا كان موجباً اطبع "موجب"
# وإلا اطبع "سالب أو صفر"
`,hint:"إذا (رقم > 0) ... وإلا ... نهاية",tips:["وإلا يُنفَّذ فقط عندما يكون الشرط خاطئاً","وإلا اختيارية — يمكنك استخدام إذا بدونها"],concepts:["وإلا","if/else"]},{id:"l1_27",title:"إذا / وإلا إذا — مسارات متعددة",desc:"عندما يكون لديك أكثر من خيارين",explanation:'ماذا لو كانت لديك أكثر من حالتين؟ مثلاً، تصنيف الدرجات: ممتاز، جيد جداً، جيد، مقبول، راسب — خمس حالات. هنا نستخدم `وإلا إذا` لإضافة شروط إضافية. الحاسوب يفحص الشروط بالترتيب من الأعلى للأسفل وينفذ أول شرط يتحقق.\n\nالترتيب مهم جداً! إذا وضعت `درجة >= 60` قبل `درجة >= 90`، سيحصل الطالب الحاصل على 95 على تقدير "مقبول" لأن الشرط الأول تحقق أولاً. لذا ابدأ دائماً بالشروط الأكثر تحديداً (الأعلى) ثم الأعم، وضع `وإلا` بدون شرط في النهاية لالتقاط كل الحالات المتبقية.\n\nهذا النمط (سلسلة إذا/وإلا إذا) شائع جداً في البرمجة. في الدروس القادمة ستتعلم `طابق` التي تقدم طريقة أنظف للتعامل مع الحالات المتعددة بدلاً من السلاسل الطويلة.',code:`متغير درجة = 85

إذا (درجة >= 90)
    اطبع_سطر("ممتاز A")
وإلا إذا (درجة >= 80)
    اطبع_سطر("جيد جداً B")
وإلا إذا (درجة >= 70)
    اطبع_سطر("جيد C")
وإلا إذا (درجة >= 60)
    اطبع_سطر("مقبول D")
وإلا
    اطبع_سطر("راسب F")
نهاية

# مثال: الفصول
متغير شهر = 3
إذا (شهر >= 3 و شهر <= 5)
    اطبع_سطر("الربيع 🌸")
وإلا إذا (شهر >= 6 و شهر <= 8)
    اطبع_سطر("الصيف ☀️")
وإلا إذا (شهر >= 9 و شهر <= 11)
    اطبع_سطر("الخريف 🍂")
وإلا
    اطبع_سطر("الشتاء ❄️")
نهاية`,exercise:`# تمرين: اطلب عمر المستخدم وصنّفه:
# أقل من 13: طفل
# 13-17: مراهق
# 18-64: بالغ
# 65+: كبير في السن
`,hint:"استخدم سلسلة وإلا إذا لتغطية كل الفئات",tips:["أول شرط يتحقق يُنفَّذ ويُتجاهل الباقي","وإلا في النهاية تلتقط كل الحالات المتبقية","رتّب الشروط من الأخص للأعم"],concepts:["وإلا إذا","شروط متسلسلة"]},{id:"l1_28",title:"عوامل المقارنة",desc:"قارن بين القيم: يساوي، أكبر، أصغر...",explanation:'عوامل المقارنة هي الأدوات التي نستخدمها لمقارنة قيمتين. النتيجة دائماً قيمة منطقية: `صحيح` أو `خطأ`. لدينا ستة عوامل: `==` (يساوي)، `!=` (لا يساوي)، `>` (أكبر من)، `<` (أصغر من)، `>=` (أكبر من أو يساوي)، `<=` (أصغر من أو يساوي).\n\nانتبه جيداً: `==` (علامتا يساوي) تُقارن بين قيمتين، بينما `=` (علامة واحدة) تُسند قيمة لمتغير. هذا من أكثر الأخطاء شيوعاً عند المبتدئين. `س == 5` تسأل "هل س يساوي 5؟"، أما `س = 5` فتقول "اجعل س يساوي 5".\n\nيمكنك مقارنة الأرقام والنصوص. مقارنة النصوص بـ `==` تتحقق من تطابقها حرفاً بحرف. عوامل المقارنة هي اللبنة الأساسية لبناء الشروط — كل جملة `إذا` تحتاج شرطاً يستخدم هذه العوامل.',code:`متغير س = 10
متغير ص = 20

# المساواة وعدم المساواة
اطبع_سطر("== يساوي:     " + (س == ص))   # خطأ
اطبع_سطر("!= لا يساوي:  " + (س != ص))   # صحيح

# أكبر وأصغر
اطبع_سطر("> أكبر:       " + (س > ص))    # خطأ
اطبع_سطر("< أصغر:       " + (س < ص))    # صحيح
اطبع_سطر(">= أكبر/يساوي: " + (س >= 10)) # صحيح
اطبع_سطر("<= أصغر/يساوي: " + (ص <= 20)) # صحيح

# مقارنة النصوص
متغير أ = "أحمد"
متغير ب = "أحمد"
اطبع_سطر("النصان متساويان: " + (أ == ب))  # صحيح`,exercise:`# تمرين: اطلب رقمين من المستخدم
# ثم اطبع نتيجة كل مقارنة بينهما
`,hint:"قارن بين أ و ب باستخدام ==، !=، >، <، >=، <=",tips:["== للمقارنة (هل متساويان؟) و = للإسناد (ضع القيمة)","المقارنات تُرجع قيمة منطقية: صحيح أو خطأ"],concepts:["==","!=",">","<",">=","<=","مقارنة"]},{id:"l1_29",title:"العوامل المنطقية: و، أو، ليس",desc:"ادمج شروطاً متعددة معاً",explanation:'العوامل المنطقية تسمح لك بدمج عدة شروط معاً. `و` (أو `&&`) تعني "يجب أن يتحقق الشرطان معاً". مثل: "يمكنك القيادة إذا كان عمرك >= 18 و لديك رخصة". إذا غاب أي شرط، النتيجة `خطأ`.\n\n`أو` (أو `||`) تعني "يكفي أن يتحقق أحد الشرطين". مثل: "اليوم عطلة إذا كان يوم جمعة أو يوم سبت". حتى لو تحقق واحد فقط، النتيجة `صحيح`.\n\n`ليس` (أو `!`) تعكس القيمة المنطقية. إذا كان الشرط `صحيح` يصبح `خطأ` والعكس. مثل: "إذا ليس مسجل" تعني "إذا كان غير مسجل". لغة ص تدعم الكلمات العربية (`و`، `أو`، `ليس`) والرموز (`&&`، `||`، `!`) — اختر ما يناسبك. الكلمات العربية أوضح للقراءة.',code:`متغير عمر = 25
متغير لديه_رخصة = صحيح

# و (AND) — كلاهما يجب أن يتحقق
إذا (عمر >= 18 و لديه_رخصة)
    اطبع_سطر("يمكنك القيادة ✓")
نهاية

# أو (OR) — أحدهما يكفي
متغير يوم = "جمعة"
إذا (يوم == "جمعة" أو يوم == "سبت")
    اطبع_سطر("عطلة نهاية الأسبوع! 🎉")
نهاية

# ليس (NOT) — عكس الشرط
متغير مسجل = خطأ
إذا (ليس مسجل)
    اطبع_سطر("يرجى التسجيل أولاً")
نهاية

# يمكن استخدام الرموز أيضاً: && || !
إذا (عمر >= 18 && لديه_رخصة)
    اطبع_سطر("يمكنك القيادة (بالرموز)")
نهاية`,exercise:`# تمرين: نظام تسجيل دخول بشرطين:
# 1. اسم المستخدم == "admin"
# 2. كلمة السر == "1234"
# يجب تحقق الشرطين معاً للدخول
`,hint:"استخدم و لربط الشرطين",tips:["و = &&، أو = ||، ليس = !","و: كلاهما صحيح → صحيح","أو: أحدهما صحيح → صحيح","ليس: يعكس القيمة المنطقية"],concepts:["و","أو","ليس","&&","||","!","عوامل منطقية"]},{id:"l1_30",title:"العامل الثلاثي (?:)",desc:"اختصار إذا/وإلا في سطر واحد",explanation:'العامل الثلاثي هو اختصار لجملة إذا/وإلا في سطر واحد. صيغته: `شرط ? قيمة_إذا_صحيح : قيمة_إذا_خطأ`. يسمى "ثلاثي" لأنه يأخذ ثلاثة أجزاء: الشرط، والقيمة عند الصحة، والقيمة عند الخطأ.\n\nاستخدمه للحالات البسيطة فقط. بدلاً من كتابة 5 أسطر (إذا/وإلا/نهاية) لاختيار قيمة واحدة، اكتب سطراً واحداً: `متغير حالة = عمر >= 18 ? "بالغ" : "قاصر"`. هذا يجعل الكود أقصر وأوضح في الحالات البسيطة.\n\nتجنب تداخل العوامل الثلاثية (وضع واحد داخل آخر) لأنها تصبح صعبة القراءة. إذا كان لديك أكثر من حالتين، استخدم إذا/وإلا إذا العادية. القاعدة: إذا احتجت أكثر من سطر واحد لفهم العبارة، فهي معقدة جداً للعامل الثلاثي.',code:`# الشكل: شرط ? قيمة_إذا_صحيح : قيمة_إذا_خطأ

متغير عمر = 20
متغير حالة = عمر >= 18 ? "بالغ" : "قاصر"
اطبع_سطر(حالة)  # بالغ

# مفيد في الطباعة المباشرة
متغير درجة = 75
اطبع_سطر(درجة >= 60 ? "ناجح ✓" : "راسب ✗")

# يمكن تداخله (لكن تجنب ذلك لأنه يصعب القراءة)
متغير ن = 0
متغير وصف = ن > 0 ? "موجب" : (ن < 0 ? "سالب" : "صفر")
اطبع_سطر(ن + " هو " + وصف)`,exercise:`# تمرين: اكتب سطراً واحداً يطبع:
# "زوجي" إذا كان العدد زوجياً، "فردي" إذا كان فردياً
`,hint:'عدد % 2 == 0 ? "زوجي" : "فردي"',tips:["العامل الثلاثي اختصار لـ إذا/وإلا البسيطة","لا تستخدمه لشروط معقدة — اكتب إذا/وإلا بدلاً منه",'يمكن استخدامه داخل f-string: f"الحالة: {عمر >= 18 ? \\"بالغ\\" : \\"قاصر\\"}"'],concepts:["? :","عامل ثلاثي","ternary"]},{id:"l1_31",title:"شروط متداخلة",desc:"ضع شرطاً داخل شرط للحالات المعقدة",explanation:'أحياناً تحتاج وضع شرط داخل شرط آخر. مثل: "إذا كان عمرك أقل من 18، ثم إذا كنت مع ولي أمر يمكنك الدخول، وإلا لا". الشرط الداخلي يُفحص فقط إذا تحقق الشرط الخارجي أولاً.\n\nكل شرط متداخل يحتاج كلمة `نهاية` خاصة به. لذا إذا داخلت شرطين، ستحتاج كلمتي `نهاية`. هذا قد يصبح معقداً مع كثرة التداخل، لذا يُنصح بتجنب التداخل العميق (أكثر من مستويين).\n\nالبديل الأنظف هو استخدام `وإلا إذا` أو العوامل المنطقية (`و`/`أو`) لتسطيح الشروط المتداخلة. مثلاً: بدلاً من `إذا (عمر < 18) ثم إذا (مع_ولي)`، يمكنك كتابة `إذا (عمر < 18 و مع_ولي)` في سطر واحد. دائماً فضّل الكود الأبسط والأوضح.',code:`متغير عمر = 17
متغير مع_ولي = صحيح

إذا (عمر >= 18)
    اطبع_سطر("مسموح بالدخول")
وإلا
    إذا (عمر >= 15 و مع_ولي)
        اطبع_سطر("مسموح بالدخول مع ولي الأمر")
    وإلا
        اطبع_سطر("غير مسموح بالدخول")
    نهاية
نهاية

# مثال: تصنيف الوزن
متغير وزن = 72
متغير طول_م = 1.75
متغير bmi = وزن / (طول_م ** 2)

اطبع_سطر(f"مؤشر BMI: {bmi}")
إذا (bmi < 18.5)
    اطبع_سطر("نحافة")
وإلا إذا (bmi < 25)
    اطبع_سطر("وزن طبيعي ✓")
وإلا إذا (bmi < 30)
    اطبع_سطر("وزن زائد")
وإلا
    اطبع_سطر("سمنة")
نهاية`,exercise:`# تمرين: نظام تسعير تذاكر:
# الأطفال (<12): 10 ريال
# الشباب (12-17): 25 ريال
# البالغين (18-64): 50 ريال
# كبار السن (65+): 30 ريال
# إذا كان يحمل بطاقة خصم: خصم 50% على أي سعر
`,hint:"حدد السعر أولاً بالعمر، ثم طبّق الخصم إن وُجد",tips:["تجنب التداخل العميق — استخدم وإلا إذا بدلاً منه","أعد هيكلة الشروط المعقدة في دوال منفصلة","اختبر كل مسار بقيم مختلفة"],concepts:["شروط متداخلة","if مداخل"]},{id:"l1_32",title:"تمرين: آلة حاسبة بسيطة",desc:"ابنِ آلة حاسبة تفاعلية تطبق العمليات الأربعة",explanation:'هذا التمرين يجمع كل ما تعلمته عن الشروط في تطبيق عملي — آلة حاسبة! الفكرة بسيطة: لديك رقمان وعملية حسابية، وتستخدم سلسلة من إذا/وإلا إذا لتحديد أي عملية تُنفَّذ.\n\nلاحظ كيف تعاملنا مع حالة القسمة على صفر — هذا نمط مهم يسمى "التحقق من الحالات الخاصة". دائماً فكر: هل هناك مدخلات قد تسبب خطأ؟ القسمة على صفر، نص فارغ، رقم سالب — التحقق من هذه الحالات يجعل برنامجك أكثر متانة.\n\nهذا المثال يُظهر أيضاً لماذا ستتعلم لاحقاً `طابق` — عندما يكون لديك متغير واحد تقارنه بعدة قيم، فإن سلسلة إذا/وإلا إذا تصبح طويلة. `طابق` ستوفر لك طريقة أنظف وأقصر لنفس المنطق.',code:`# آلة حاسبة بسيطة
اطبع_سطر("═══ الآلة الحاسبة ═══")

متغير أ = 10
متغير ب = 3
متغير العملية = "+"

إذا (العملية == "+")
    اطبع_سطر(f"{أ} + {ب} = {أ + ب}")
وإلا إذا (العملية == "-")
    اطبع_سطر(f"{أ} - {ب} = {أ - ب}")
وإلا إذا (العملية == "*")
    اطبع_سطر(f"{أ} × {ب} = {أ * ب}")
وإلا إذا (العملية == "/")
    إذا (ب != 0)
        اطبع_سطر(f"{أ} ÷ {ب} = {أ / ب}")
    وإلا
        اطبع_سطر("خطأ: لا يمكن القسمة على صفر!")
    نهاية
وإلا
    اطبع_سطر("عملية غير معروفة: " + العملية)
نهاية`,exercise:`# تمرين: وسّع الآلة الحاسبة لتشمل:
# 1. عملية الأس (**)
# 2. عملية باقي القسمة (%)
# 3. اجعلها تفاعلية (تقرأ من المستخدم)
`,hint:"أضف حالات وإلا إذا جديدة للعمليات الإضافية",tips:["تحقق دائماً من القسمة على صفر","حوّل إدخال المستخدم لأرقام بـ لرقم()","هذا النمط (إذا/وإلا إذا متعدد) سنستبدله لاحقاً بـ طابق"],concepts:["تطبيق عملي","شروط","حاسبة"]}]},ec={id:"l1_s5",title:"الحلقات والتكرار",icon:"🔄",lessons:[{id:"l1_33",title:"حلقة بينما (while)",desc:"كرّر كوداً ما دام الشرط متحققاً",explanation:'الحلقات هي من أقوى أدوات البرمجة — تسمح لك بتكرار كود معين عدة مرات بدون نسخه يدوياً. حلقة `بينما` هي الأبسط: طالما الشرط صحيح، استمر في التنفيذ. تخيّل أنك تقول للحاسوب: "استمر في العدّ طالما العداد أقل من 5". هذا بالضبط ما يفعله الكود. الخطأ الشائع هو نسيان تحديث المتغير داخل الحلقة — فتصبح الحلقة لانهائية!',code:`# العد من 1 إلى 5
متغير عداد = 1
بينما (عداد <= 5)
    اطبع_سطر("العداد: " + عداد)
    عداد += 1
نهاية

# مثال عملي: مضاعفة حتى حد
متغير قيمة = 1
بينما (قيمة < 1000)
    اطبع(قيمة + " ")
    قيمة *= 2
نهاية
اطبع_سطر("")
# الخرج: 1 2 4 8 16 32 64 128 256 512`,exercise:`# تمرين: اطبع العد التنازلي من 10 إلى 1
# ثم اطبع "🚀 انطلاق!"
`,hint:"ابدأ من 10 واطرح 1 في كل دورة",tips:["تأكد من تغيير المتغير داخل الحلقة وإلا ستدور للأبد!","بينما و طالما مترادفتان — استخدم أيهما تفضل"],concepts:["بينما","while","تكرار"]},{id:"l1_34",title:"حلقة لكل (for each)",desc:"التكرار على عناصر مصفوفة أو مجموعة",explanation:'حلقة `لكل` هي الطريقة المثلى للتكرار على مجموعة عناصر — مصفوفة أو نص أو نطاق. بدلاً من إدارة عداد بنفسك، تقول ببساطة: "لكل عنصر في هذه المجموعة، افعل كذا". المتغير (مثل `فاكهة`) يأخذ تلقائياً قيمة عنصر مختلف في كل دورة. هذا أسهل وأقل عرضة للأخطاء من استخدام `بينما` مع عداد يدوي.',code:`# التكرار على مصفوفة
متغير فواكه = ["تفاح", "موز", "برتقال", "عنب"]
لكل فاكهة في فواكه
    اطبع_سطر("أحب " + فاكهة)
نهاية

# التكرار على نص (حرفاً حرفاً)
لكل حرف في "مرحبا"
    اطبع(حرف + "-")
نهاية
اطبع_سطر("")  # م-ر-ح-ب-ا-

# التكرار مع مصفوفة أرقام
متغير درجات = [90, 85, 78, 92, 88]
متغير مجموع_د = 0
لكل درجة في درجات
    مجموع_د += درجة
نهاية
اطبع_سطر(f"المتوسط: {مجموع_د / طول(درجات)}")`,exercise:`# تمرين: لديك مصفوفة أسعار [100, 200, 50, 300, 150]
# 1. اطبع كل سعر
# 2. احسب المجموع
# 3. أوجد أعلى سعر
`,hint:"استخدم لكل للتكرار ومتغيرات لتتبع المجموع والأعلى",tips:["لكل أبسط وأوضح من بينما للتكرار على مجموعات","المتغير (فاكهة) يأخذ قيمة عنصر مختلف في كل دورة","لا حاجة لعداد — اللغة تديره تلقائياً"],concepts:["لكل","في","for each"]},{id:"l1_35",title:"النطاقات — مدى()",desc:"أنشئ تسلسل أرقام للتكرار",explanation:"النطاقات تُنشئ تسلسل أرقام بسهولة للتكرار عليه. `مدى(5)` يعطيك 0، 1، 2، 3، 4 (خمسة أرقام تبدأ من الصفر). `مدى(1, 6)` يعطيك 1، 2، 3، 4، 5 (من 1 إلى 5). لاحظ أن النهاية دائماً غير مشمولة — وهذا مقصود لأن `مدى(5)` يعطيك 5 عناصر بالضبط. صيغة النقطتين `1..6` اختصار أنيق لنفس الشيء.",code:`# مدى(نهاية) — من 0 إلى نهاية-1
لكل ع في مدى(5)
    اطبع(ع + " ")  # 0 1 2 3 4
نهاية
اطبع_سطر("")

# مدى(بداية, نهاية) — من بداية إلى نهاية-1
لكل ع في مدى(1, 6)
    اطبع(ع + " ")  # 1 2 3 4 5
نهاية
اطبع_سطر("")

# صيغة النطاق: بداية..نهاية
لكل ع في 1..6
    اطبع(ع + " ")  # 1 2 3 4 5
نهاية
اطبع_سطر("")

# تطبيق: جدول الضرب
متغير العدد = 7
لكل ع في 1..11
    اطبع_سطر(f"{العدد} × {ع} = {العدد * ع}")
نهاية`,exercise:`# تمرين: اطبع الأعداد الزوجية من 2 إلى 20
# ثم اطبع مجموعها
`,hint:"يمكنك استخدام مدى مع شرط: إذا (ع % 2 == 0)",tips:["مدى(5) = من 0 إلى 4 (لا يشمل 5)","مدى(1, 10) = من 1 إلى 9","1..10 مثل مدى(1, 10)"],concepts:["مدى","range","..","نطاق"]},{id:"l1_36",title:"توقف واستمر",desc:"تحكّم في تدفق الحلقة: أوقفها أو تخطَّ دورة",explanation:"أحياناً تحتاج للتحكم في الحلقة أثناء تنفيذها. `توقف` تُنهي الحلقة فوراً وتخرج منها — مفيدة عندما تجد ما تبحث عنه ولا حاجة للاستمرار. `استمر` تتخطى بقية الدورة الحالية وتنتقل للدورة التالية — مفيدة لتجاهل عناصر معينة. تخيّل أنك تبحث في صندوق: `توقف` = وجدت الشيء وأغلقت الصندوق، `استمر` = هذا ليس ما أبحث عنه، أتابع البحث.",code:`# توقف — اخرج من الحلقة فوراً
اطبع_سطر("البحث عن أول عدد قابل للقسمة على 7:")
لكل ع في 1..100
    إذا (ع % 7 == 0)
        اطبع_سطر("وجدت: " + ع)
        توقف    # أول واحد يكفي
    نهاية
نهاية

# استمر — تخطَّ بقية الدورة الحالية
اطبع_سطر("الأعداد غير القابلة للقسمة على 3:")
لكل ع في 1..20
    إذا (ع % 3 == 0)
        استمر    # تخطَّ مضاعفات 3
    نهاية
    اطبع(ع + " ")
نهاية
اطبع_سطر("")`,exercise:`# تمرين: ابحث عن أول 5 أعداد أولية
# (العدد الأولي يقبل القسمة على 1 وعلى نفسه فقط)
`,hint:"استخدم عداداً وتوقف عندما تجد 5 أعداد، واستمر لتخطي غير الأولية",tips:["توقف يُنهي الحلقة بالكامل","استمر يتخطى بقية الدورة الحالية فقط ويبدأ الدورة التالية","لا تستخدمهما بكثرة — أحياناً إعادة صياغة الشرط أوضح"],concepts:["توقف","استمر","break","continue"]},{id:"l1_37",title:"حلقات متداخلة",desc:"حلقة داخل حلقة — لمصفوفات ثنائية الأبعاد",explanation:"الحلقات المتداخلة هي حلقة داخل حلقة أخرى. تُستخدم لأي شيء له بُعدان: صفوف وأعمدة في جدول، أو إحداثيات (س، ص) في شبكة. الحلقة الخارجية تتحكم في الصفوف، والداخلية في الأعمدة. لكل دورة في الحلقة الخارجية، تُنفَّذ الحلقة الداخلية بالكامل. مثلاً: جدول 5×5 = 25 عملية (5 صفوف × 5 أعمدة).",code:`# جدول الضرب الكامل (1-5)
لكل ص في 1..6
    لكل ع في 1..6
        متغير نتيجة = ص * ع
        إذا (نتيجة < 10)
            اطبع(" " + نتيجة + " ")
        وإلا
            اطبع(نتيجة + " ")
        نهاية
    نهاية
    اطبع_سطر("")
نهاية

# أنماط نجمية
لكل سطر في 1..6
    لكل نجمة في 1..(سطر + 1)
        اطبع("★ ")
    نهاية
    اطبع_سطر("")
نهاية`,exercise:`# تمرين: اطبع مثلث أرقام:
# 1
# 1 2
# 1 2 3
# 1 2 3 4
# 1 2 3 4 5
`,hint:"حلقة خارجية للأسطر، داخلية للأرقام في كل سطر",tips:["الحلقة الخارجية تتحكم في الأسطر","الحلقة الداخلية تتحكم في الأعمدة/العناصر","كل دورة خارجية تُشغّل الحلقة الداخلية بالكامل"],concepts:["حلقات متداخلة","أنماط"]},{id:"l1_38",title:"تمرين: جدول الضرب التفاعلي",desc:"ابنِ برنامجاً يطبع جدول ضرب لأي عدد",explanation:"هذا التمرين يجمع عدة مفاهيم: الحلقات للتكرار، النطاقات لإنشاء تسلسل الأرقام، و f-string للتنسيق الجميل. التحدي هو جعل الجدول متناسقاً بصرياً — لاحظ استخدام العامل الثلاثي لإضافة مسافات حسب طول الرقم. هذا النوع من التنسيق مهم جداً في البرامج الحقيقية.",code:`# جدول ضرب تفاعلي
متغير العدد = 9

اطبع_سطر("╔═══════════════════╗")
اطبع_سطر(f"║  جدول ضرب {العدد}       ║")
اطبع_سطر("╠═══════════════════╣")

لكل ع في 1..13
    متغير ناتج = العدد * ع
    متغير مسافة = ناتج < 10 ? "  " : (ناتج < 100 ? " " : "")
    اطبع_سطر(f"║  {العدد} × {ع < 10 ? ' ' + لنص(ع) : لنص(ع)} = {مسافة}{ناتج}  ║")
نهاية

اطبع_سطر("╚═══════════════════╝")`,exercise:`# تمرين: اطبع جداول الضرب من 1 إلى 10
# اجعل كل جدول يطبع في صف واحد:
# 1×1=1  1×2=2  1×3=3 ...
# 2×1=2  2×2=4  2×3=6 ...
`,hint:"حلقة خارجية للعدد (1-10) وداخلية للمضروب (1-10)",tips:["f-string مفيدة جداً لتنسيق الجداول","يمكنك التحكم في المسافات باستخدام شروط على طول الرقم"],concepts:["تطبيق عملي","حلقات متداخلة","تنسيق"]},{id:"l1_39",title:"تمرين: أنماط نجمية",desc:"ارسم أشكالاً هندسية بالنجوم والرموز",explanation:"رسم الأنماط بالنجوم تمرين كلاسيكي يعلّمك التفكير الخوارزمي — تحليل الشكل إلى قواعد رياضية. مثلاً: لرسم مثلث، لاحظ أن عدد النجوم يزداد بواحد كل سطر، وعدد المسافات يقل بواحد. المفتاح هو إيجاد العلاقة الرياضية بين رقم السطر وعدد الرموز. هذا النوع من التفكير أساسي للبرمجة.",code:`# مثلث متساوي الساقين
متغير الارتفاع = 5
لكل ع في 1..(الارتفاع + 1)
    # مسافات قبل النجوم
    لكل فراغ في 1..(الارتفاع - ع + 1)
        اطبع(" ")
    نهاية
    # النجوم
    لكل نجمة في 1..(2 * ع)
        اطبع("★")
    نهاية
    اطبع_سطر("")
نهاية

# مربع مفرغ
اطبع_سطر("\\nمربع مفرغ:")
متغير الحجم = 5
لكل ص في 1..(الحجم + 1)
    لكل ع في 1..(الحجم + 1)
        إذا (ص == 1 أو ص == الحجم أو ع == 1 أو ع == الحجم)
            اطبع("■ ")
        وإلا
            اطبع("  ")
        نهاية
    نهاية
    اطبع_سطر("")
نهاية`,exercise:`# تمرين: ارسم ماسة (◆) بارتفاع 7:
#    ★
#   ★★★
#  ★★★★★
# ★★★★★★★
#  ★★★★★
#   ★★★
#    ★
`,hint:"النصف العلوي مثلث يكبر، والنصف السفلي مثلث يصغر",tips:["قسّم المشكلة: نصف علوي + نصف سفلي","عدد المسافات = الارتفاع - رقم_السطر","عدد النجوم = 2 × رقم_السطر - 1"],concepts:["أنماط","تفكير خوارزمي","حلقات"]},{id:"l1_40",title:"🎓 مراجعة شاملة — مشروع مصغر",desc:"اجمع كل ما تعلمته في مشروع واحد!",explanation:"مبروك على وصولك لهذا الدرس! هذا المشروع يجمع كل ما تعلمته في المستوى الأول: المتغيرات لتخزين البيانات، الحلقات للتكرار، الشروط لاتخاذ القرارات، و f-string للتنسيق. لعبة تخمين الرقم نموذج ممتاز لأنها تتضمن: حلقة تستمر حتى الفوز، شروط للمقارنة، عداد للمحاولات، وعدة مخرجات حسب الحالة. إتقان هذا المشروع يعني أنك جاهز للمستوى الثاني!",code:`# 🎮 لعبة تخمين الرقم
اطبع_سطر("═══ لعبة تخمين الرقم ═══")
اطبع_سطر("خمّن رقماً بين 1 و 100")

ثابت الرقم_السري = 42    # في التطبيق الحقيقي: عشوائي()
متغير محاولات = 0
متغير فاز = خطأ

بينما (ليس فاز)
    محاولات += 1
    متغير تخمين = 50    # في التطبيق الحقيقي: لرقم(اقرأ())
    
    إذا (تخمين == الرقم_السري)
        اطبع_سطر(f"🎉 أحسنت! الرقم هو {الرقم_السري}")
        اطبع_سطر(f"عدد المحاولات: {محاولات}")
        فاز = صحيح
    وإلا إذا (تخمين < الرقم_السري)
        اطبع_سطر("⬆️ أكبر!")
    وإلا
        اطبع_سطر("⬇️ أصغر!")
    نهاية
    
    # حد أقصى للمحاولات
    إذا (محاولات >= 10 و ليس فاز)
        اطبع_سطر(f"😔 انتهت المحاولات! الرقم كان {الرقم_السري}")
        توقف
    نهاية
نهاية`,exercise:`# تمرين نهائي: ابنِ برنامج "دفتر الدرجات":
# 1. عرّف مصفوفة بأسماء 5 طلاب ودرجاتهم
# 2. احسب المتوسط
# 3. أوجد أعلى وأدنى درجة
# 4. صنّف كل طالب (ممتاز/جيد/...)
# 5. اطبع تقريراً منسقاً
`,hint:"ادمج المصفوفات، الحلقات، الشروط، والنصوص المنسقة",tips:["قسّم المشروع لخطوات صغيرة","اختبر كل جزء على حدة","مبروك! أنت جاهز للمستوى الثاني 🎉"],concepts:["مراجعة شاملة","مشروع","كل المفاهيم الأساسية"]}]},tc={id:1,title:"من الصفر",titleEn:"From Zero",description:"ابدأ رحلتك البرمجية — لا خبرة سابقة مطلوبة",icon:"🌱",color:"#10b981",sections:[Kl,Yl,Xl,Zl,ec]},nc={id:"l2_s1",title:"الدوال",icon:"⚡",lessons:[{id:"l2_01",title:"تعريف دالة واستدعاؤها",desc:"الدالة كتلة كود قابلة لإعادة الاستخدام",explanation:"الدالة مثل الوصفة في كتاب الطبخ — تكتبها مرة واحدة ثم تستخدمها كلما احتجتها. بدلاً من تكرار نفس الأسطر البرمجية في عدة أماكن، نجمعها في دالة ونستدعيها باسمها. هذا يجعل الكود أقصر وأسهل للفهم والصيانة.",code:`# تعريف دالة بسيطة
دالة تحية()
اطبع_سطر("مرحبا من الدالة!")
نهاية

# استدعاء الدالة
تحية()
تحية()    # يمكن استدعاؤها عدة مرات

# دالة بسطر واحد من المنطق
دالة خط_فاصل()
اطبع_سطر("═══════════════")
نهاية

خط_فاصل()
اطبع_سطر("محتوى مهم")
خط_فاصل()`,exercise:`# تمرين: اكتب دالة تطبع مربعاً من النجوم بحجم 5×5
# ثم استدعها 3 مرات
`,hint:"دالة مربع_نجوم() ... لكل ... نهاية",tips:["الدالة تُعرَّف مرة وتُستدعى مرات","اسم الدالة يكون وصفياً: احسب_المتوسط، اطبع_تقرير","لا تنسَ الأقواس () عند الاستدعاء"],concepts:["دالة","استدعاء","تعريف دالة"]},{id:"l2_02",title:"المعاملات (Parameters)",desc:"مرّر بيانات للدالة عبر المعاملات",explanation:'المعاملات هي "المدخلات" التي تعطيها للدالة لتعمل عليها — مثل إعطاء الطباخ المكونات. الدالة تأخذ هذه القيم وتستخدمها في حساباتها. يمكنك تمرير معامل واحد أو عدة معاملات حسب ما تحتاجه الدالة.',code:`# دالة بمعامل واحد
دالة تحية_شخصية(الاسم)
اطبع_سطر(f"مرحبا يا {الاسم}!")
نهاية

تحية_شخصية("أحمد")
تحية_شخصية("سارة")

# دالة بمعاملين
دالة جمع(أ، ب)
اطبع_سطر(f"{أ} + {ب} = {أ + ب}")
نهاية

جمع(10, 20)
جمع(3, 7)

# دالة بعدة معاملات
دالة معلومات_شخص(الاسم, العمر, المدينة)
اطبع_سطر(f"{الاسم} — {العمر} سنة — {المدينة}")
نهاية

معلومات_شخص("محمد", 25, "الرياض")`,exercise:`# تمرين: اكتب دالة تحسب مساحة مثلث
# المعاملات: القاعدة والارتفاع
# المساحة = (القاعدة × الارتفاع) / 2
`,hint:"دالة مساحة_مثلث(قاعدة, ارتفاع)",tips:["المعاملات متغيرات محلية داخل الدالة","ترتيب المعاملات مهم عند الاستدعاء","الفاصلة ، أو , كلاهما يعمل"],concepts:["معاملات","وسائط","parameters"]},{id:"l2_03",title:"القيمة المُرجعة (return)",desc:"أرجع نتيجة من الدالة بكلمة ارجع",explanation:'كلمة "ارجع" تجعل الدالة تعطيك نتيجة يمكنك استخدامها لاحقاً — مثل الآلة الحاسبة التي تُرجع الإجابة. بدون "ارجع"، الدالة تنفذ عملاً لكن لا تعطيك شيئاً تخزنه. القيمة المُرجعة يمكن تخزينها في متغير أو استخدامها مباشرة في عمليات أخرى.',code:`# دالة تُرجع قيمة
دالة مربع(عدد)
ارجع عدد ** 2
نهاية

متغير نتيجة = مربع(5)
اطبع_سطر("مربع 5 = " + نتيجة)  # 25

# استخدام النتيجة مباشرة
اطبع_سطر("مربع 8 = " + مربع(8))

# دالة تُرجع نتيجة عملية
دالة متوسط(أ, ب)
ارجع (أ + ب) / 2
نهاية

اطبع_سطر("المتوسط: " + متوسط(80, 90))

# تسلسل استدعاءات
اطبع_سطر("مربع المتوسط: " + مربع(متوسط(4, 6)))`,exercise:`# تمرين: اكتب دالة "أكبر_بين_ثلاثة" تستقبل 3 أرقام
# وترجع الأكبر بينها
`,hint:"قارن الأرقام باستخدام إذا/وإلا إذا",tips:["ارجع تُنهي تنفيذ الدالة فوراً","يمكن أن تُرجع أي نوع: رقم، نص، منطقي، مصفوفة...","دالة بدون ارجع تُرجع لاشيء"],concepts:["ارجع","return","قيمة مرجعة"]},{id:"l2_04",title:"القيم الافتراضية للمعاملات",desc:"حدّد قيماً افتراضية للمعاملات الاختيارية",explanation:"القيم الافتراضية تجعل بعض المعاملات اختيارية — إذا لم تمررها، تستخدم الدالة القيمة الافتراضية. مثل طلب القهوة: إذا لم تحدد الحجم، يعطونك الحجم العادي. هذا يجعل استدعاء الدالة أسهل في الحالات الشائعة مع الاحتفاظ بالمرونة.",code:`# معامل بقيمة افتراضية
دالة تحية(الاسم = "زائر")
اطبع_سطر(f"مرحبا يا {الاسم}!")
نهاية

تحية("أحمد")    # مرحبا يا أحمد!
تحية()           # مرحبا يا زائر!

# عدة معاملات مع افتراضيات
دالة اطبع_فاتورة(المنتج, السعر, الكمية = 1, الخصم = 0)
متغير إجمالي = السعر * الكمية * (1 - الخصم / 100)
اطبع_سطر(f"{المنتج}: {إجمالي} ريال")
نهاية

اطبع_فاتورة("كتاب", 50)           # كمية 1، بدون خصم
اطبع_فاتورة("قلم", 10, 5)         # 5 أقلام، بدون خصم
اطبع_فاتورة("دفتر", 20, 3, 10)    # 3 دفاتر، خصم 10%`,exercise:`# تمرين: اكتب دالة "تقييم" تستقبل:
# - الدرجة (إلزامي)
# - الحد_الأقصى (افتراضي 100)
# ترجع النسبة المئوية والتقدير
`,hint:"دالة تقييم(درجة, حد_أقصى = 100)",tips:["المعاملات الإلزامية أولاً، ثم الاختيارية","القيمة الافتراضية تُستخدم فقط إذا لم يُمرر المعامل"],concepts:["قيم افتراضية","معاملات اختيارية"]},{id:"l2_05",title:"الدوال بدون قيمة مرجعة",desc:"دوال تُنفذ إجراءً دون إرجاع قيمة",explanation:'ليست كل الدوال تحتاج أن تُرجع شيئاً — بعضها يكتفي بتنفيذ عمل معين مثل الطباعة أو تعديل بيانات. هذه تُسمى "دوال إجرائية". مثل الغسالة: تغسل الملابس لكن لا تُرجع لك رقماً، بل تنجز المهمة فقط.',code:`# دالة إجرائية — تنفذ عملاً بدون إرجاع
دالة اطبع_شريط(الرمز, الطول)
لكل ع في مدى(الطول)
    اطبع(الرمز)
نهاية
اطبع_سطر("")
نهاية

اطبع_شريط("═", 20)
اطبع_شريط("★", 10)

# دالة تعديل سلوك
دالة تسجيل(الرسالة, المستوى = "معلومة")
إذا (المستوى == "خطأ")
    اطبع_سطر(f"❌ خطأ: {الرسالة}")
وإلا إذا (المستوى == "تحذير")
    اطبع_سطر(f"⚠️ تحذير: {الرسالة}")
وإلا
    اطبع_سطر(f"ℹ️ {الرسالة}")
نهاية
نهاية

تسجيل("بدء التشغيل")
تسجيل("ملف غير موجود", "تحذير")
تسجيل("فشل الاتصال", "خطأ")`,exercise:`# تمرين: اكتب دالة "اطبع_جدول" تستقبل مصفوفة أسماء ومصفوفة درجات
# وتطبعها في جدول منسق مع الترقيم
`,hint:"استخدم حلقة وf-string لتنسيق الصفوف",tips:["ليست كل دالة تحتاج ارجع","دوال الطباعة والتعديل عادة لا ترجع قيمة","الدالة بلا ارجع ترجع لاشيء تلقائياً"],concepts:["دوال إجرائية","void","فراغ"]},{id:"l2_06",title:"نطاق المتغيرات (Scope)",desc:"أين يمكن الوصول للمتغير؟ المحلي \uFEFFvs العام",explanation:'النطاق يحدد "أين يعيش المتغير" — المتغير المحلي داخل الدالة يختفي عند انتهائها، بينما العام يبقى متاحاً في كل مكان. فكّر بالمحلي كملاحظة على ورقة ترميها بعد الاجتماع، والعام كلوحة إعلانات الجميع يراها. فهم النطاق يمنع أخطاء مربكة كثيرة!',code:`# متغير عام — يمكن الوصول إليه من أي مكان
متغير رسالة_عامة = "أنا عام"

دالة اختبار_نطاق()
# متغير محلي — فقط داخل هذه الدالة
متغير رسالة_محلية = "أنا محلي"
اطبع_سطر(رسالة_عامة)     # ✓ يمكن الوصول للعام
اطبع_سطر(رسالة_محلية)     # ✓ يمكن الوصول للمحلي
نهاية

اختبار_نطاق()
اطبع_سطر(رسالة_عامة)    # ✓ عام
# اطبع_سطر(رسالة_محلية)  # ❌ خطأ! المحلي غير مرئي هنا

# المتغيرات المحلية لا تؤثر على العامة
متغير عداد = 100
دالة زيادة()
متغير عداد = 0    # متغير محلي مختلف!
عداد += 1
اطبع_سطر("المحلي: " + عداد)   # 1
نهاية

زيادة()
اطبع_سطر("العام: " + عداد)   # 100 — لم يتأثر!`,exercise:`# تمرين: ماذا يطبع هذا البرنامج؟ خمّن ثم تحقق:
# متغير x = 10
# دالة أ()
#     متغير x = 20
#     اطبع_سطر(x)
# نهاية
# أ()
# اطبع_سطر(x)
`,hint:"المتغير المحلي داخل الدالة يُخفي العام",tips:["المتغيرات المحلية تختفي عند خروج الدالة","نفس الاسم داخل الدالة ينشئ متغيراً جديداً لا يؤثر على الخارجي","قلّل استخدام المتغيرات العامة"],concepts:["نطاق","محلي","عام","scope"]},{id:"l2_07",title:"الدوال المتداخلة",desc:"عرّف دالة داخل دالة أخرى",explanation:"يمكنك تعريف دالة مساعدة داخل دالة أكبر — مثل فريق العمل الذي فيه مساعدون. الدالة الداخلية مخفية عن العالم الخارجي ولا يمكن استدعاؤها إلا من داخل الدالة الحاوية. هذا يساعد على تنظيم المنطق المعقد وإبقاء الكود نظيفاً.",code:`# دالة داخل دالة
دالة حاسبة_ضريبة(المبلغ, نسبة_ضريبة)
# دالة مساعدة داخلية
دالة احسب_ضريبة(مبلغ)
    ارجع مبلغ * نسبة_ضريبة / 100
نهاية

متغير الضريبة = احسب_ضريبة(المبلغ)
متغير الإجمالي = المبلغ + الضريبة

اطبع_سطر(f"المبلغ: {المبلغ}")
اطبع_سطر(f"الضريبة ({نسبة_ضريبة}%): {الضريبة}")
اطبع_سطر(f"الإجمالي: {الإجمالي}")
ارجع الإجمالي
نهاية

حاسبة_ضريبة(1000, 15)`,exercise:`# تمرين: اكتب دالة "تحويل_درجة" بداخلها:
# - دالة "إلى_فهرنهايت(سيلسيوس)"
# - دالة "إلى_سيلسيوس(فهرنهايت)"
# K→C: c = k - 273.15, C→F: f = c * 9/5 + 32
`,hint:"عرّف الدوال المساعدة داخل الدالة الرئيسية",tips:["الدوال الداخلية مرئية فقط داخل الدالة الحاوية","تُستخدم لتنظيم المنطق المعقد","الدالة الداخلية يمكنها الوصول لمتغيرات الخارجية (closure)"],concepts:["دوال متداخلة","تنظيم الكود"]},{id:"l2_08",title:"الدالة كقيمة (First-Class)",desc:"الدوال قيم — خزّنها في متغيرات ومرّرها كمعاملات",explanation:'في لغة ص، الدوال ليست مجرد "أوامر" بل هي قيم حقيقية مثل الأرقام والنصوص! يمكنك تخزين دالة في متغير، تمريرها لدالة أخرى، أو حتى إرجاعها من دالة. هذا يفتح باباً واسعاً للبرمجة المرنة والقوية — أساس البرمجة الوظيفية.',code:`# تخزين دالة في متغير
دالة مربع(ن)
ارجع ن ** 2
نهاية

دالة مكعب(ن)
ارجع ن ** 3
نهاية

# متغير يحمل دالة
متغير عملية = مربع
اطبع_سطر(عملية(5))    # 25

عملية = مكعب
اطبع_سطر(عملية(5))    # 125

# تمرير دالة كمعامل
دالة طبّق(الدالة, القيمة)
ارجع الدالة(القيمة)
نهاية

اطبع_سطر(طبّق(مربع, 4))   # 16
اطبع_سطر(طبّق(مكعب, 3))   # 27`,exercise:`# تمرين: اكتب دالة "طبق_على_مصفوفة" تستقبل مصفوفة ودالة
# وتطبق الدالة على كل عنصر وترجع مصفوفة جديدة
`,hint:"كرّر على المصفوفة وطبّق الدالة على كل عنصر",tips:['الدوال في لغة ص "مواطنات من الدرجة الأولى"',"يمكن تخزينها ومرّيرها مثل أي قيمة أخرى","هذا أساس البرمجة الوظيفية"],concepts:["first-class functions","دوال كقيم","تمرير دوال"]},{id:"l2_09",title:"الإغلاقات (Closures)",desc:"الدالة تتذكر بيئتها — حتى بعد خروج الدالة الحاوية",explanation:'الإغلاق سحر البرمجة! عندما تُرجع دالة من داخل دالة أخرى، الدالة المُرجعة "تتذكر" المتغيرات من بيئتها الأصلية. مثل الصندوق السري الذي يحتفظ بشيء ثمين — كل نسخة من الدالة لها صندوقها الخاص. هذا يمكّنك من إنشاء "مصانع دوال" ذكية.',code:`# إغلاق: الدالة الداخلية تتذكر متغيرات الخارجية
دالة عداد_جديد()
متغير القيمة = 0

دالة زيادة()
    القيمة += 1
    ارجع القيمة
نهاية

ارجع زيادة
نهاية

متغير عداد1 = عداد_جديد()
اطبع_سطر(عداد1())    # 1
اطبع_سطر(عداد1())    # 2
اطبع_سطر(عداد1())    # 3

# كل عداد مستقل!
متغير عداد2 = عداد_جديد()
اطبع_سطر(عداد2())    # 1 (مستقل)

# مثال عملي: مُضاعِف
دالة مضاعف(العامل)
دالة اضرب(عدد)
    ارجع عدد * العامل
نهاية
ارجع اضرب
نهاية

متغير ضرب3 = مضاعف(3)
متغير ضرب5 = مضاعف(5)
اطبع_سطر(ضرب3(10))   # 30
اطبع_سطر(ضرب5(10))   # 50`,exercise:`# تمرين: اكتب دالة "منشئ_نطاق" تستقبل حداً أقصى
# وترجع دالة تتحقق هل الرقم داخل النطاق
`,hint:"الدالة المُرجعة تتذكر الحد الأقصى من بيئتها",tips:["الإغلاق = دالة + بيئتها (المتغيرات المحيطة)","كل استدعاء ينشئ بيئة مستقلة",'نمط قوي لإنشاء "مصانع دوال"'],concepts:["إغلاق","closure","بيئة الدالة"]},{id:"l2_10",title:"الدالة الرئيسية وتمرين شامل",desc:"نقطة البداية الرسمية للبرنامج + تمرين",explanation:'دالة "رئيسية" هي الباب الرئيسي لبرنامجك — المكان الذي يبدأ منه كل شيء. تنظيم كودك داخل دوال ثم استدعاؤها من رئيسية يجعل البرنامج واضحاً ومنظماً. الآن بعد إتقانك للدوال، أنت جاهز لبناء برامج حقيقية!',code:`# الدالة الرئيسية — نقطة دخول البرنامج
دالة رئيسية()
اطبع_سطر("بدأ البرنامج")

# حاسبة إحصائية
دالة متوسط(أرقام)
    متغير مج = 0
    لكل ر في أرقام
        مج += ر
    نهاية
    ارجع مج / طول(أرقام)
نهاية

دالة أكبر_عنصر(أرقام)
    متغير الأكبر = أرقام[0]
    لكل ر في أرقام
        إذا (ر > الأكبر)
            الأكبر = ر
        نهاية
    نهاية
    ارجع الأكبر
نهاية

متغير درجات = [85, 92, 78, 95, 88, 76, 90]
اطبع_سطر(f"المتوسط: {متوسط(درجات)}")
اطبع_سطر(f"الأعلى: {أكبر_عنصر(درجات)}")

اطبع_سطر("انتهى البرنامج")
نهاية

رئيسية()`,exercise:`# تمرين شامل: ابنِ "حاسبة إحصائية" تشمل:
# - دالة مجموع(أرقام)
# - دالة متوسط(أرقام)
# - دالة أكبر(أرقام)
# - دالة أصغر(أرقام)
# - دالة تقرير(أرقام) تطبع كل الإحصائيات
`,hint:"عرّف كل دالة على حدة ثم استدعها من التقرير",tips:["رئيسية() اختيارية — يُنفَّذ الكود بدونها تلقائياً","تنظيم الكود في دوال = كود أنظف وأسهل صيانة"],concepts:["رئيسية","main","تطبيق شامل"]}]},sc={id:"l2_s2",title:"المصفوفات والقوائم",icon:"📦",lessons:[{id:"l2_11",title:"إنشاء المصفوفات",desc:"المصفوفة تخزّن مجموعة عناصر مرتبة",explanation:"المصفوفة مثل الدرج المنظم — كل خانة لها رقم (فهرس) وتحتوي عنصراً واحداً. بدلاً من إنشاء متغير لكل عنصر (طالب١، طالب٢...)، نجمعهم في مصفوفة واحدة. المصفوفات أساس كل برنامج يتعامل مع بيانات متعددة!",code:`# إنشاء مصفوفة
متغير أرقام = [10, 20, 30, 40, 50]
متغير أسماء = ["أحمد", "سارة", "محمد"]
متغير خليط = [1, "نص", صحيح, 3.14]

# مصفوفة فارغة
متغير فارغة = []

اطبع_سطر(أرقام)
اطبع_سطر(أسماء)
اطبع_سطر("عدد العناصر: " + طول(أرقام))`,exercise:`# تمرين: أنشئ مصفوفة بأسماء 5 مدن عربية
# واطبع عددها وأول وآخر مدينة
`,hint:"استخدم [0] للأول و[طول-1] للأخير",tips:["المصفوفة تخزّن أي نوع من القيم","العناصر مرتبة ويمكن الوصول بالفهرس","الفهرسة تبدأ من 0"],concepts:["مصفوفة","إنشاء","[]"]},{id:"l2_12",title:"الوصول للعناصر وتعديلها",desc:"اقرأ وعدّل عناصر المصفوفة بالفهرس",explanation:'كل عنصر في المصفوفة له "عنوان" رقمي يبدأ من صفر (0=الأول). الفهرس السالب يعدّ من النهاية: [-1] = الأخير. فكّر بالفهرس كرقم المقعد في المسرح — تستخدمه للوصول لشخص معين بسرعة.',code:`متغير فواكه = ["تفاح", "موز", "برتقال", "عنب", "مانجو"]

# القراءة بالفهرس (يبدأ من 0)
اطبع_سطر("الأول: " + فواكه[0])     # تفاح
اطبع_سطر("الثالث: " + فواكه[2])    # برتقال
اطبع_سطر("الأخير: " + فواكه[4])    # مانجو

# الفهرس السالب — من النهاية
اطبع_سطر("الأخير: " + فواكه[-1])   # مانجو
اطبع_سطر("ما قبل الأخير: " + فواكه[-2])

# تعديل عنصر
فواكه[1] = "فراولة"
اطبع_سطر("بعد التعديل: " + فواكه)`,exercise:`# تمرين: أنشئ مصفوفة بدرجات 5 طلاب
# عدّل درجة الطالب الثالث لتصبح 100
# اطبع المصفوفة قبل وبعد
`,hint:"درجات[2] = 100",tips:["الفهرس السالب يعدّ من النهاية: [-1] = الأخير","الوصول لفهرس خارج الحدود يسبب خطأ"],concepts:["فهرسة","[i]","[-1]","تعديل عنصر"]},{id:"l2_13",title:"إضافة وحذف عناصر",desc:"أضف وأزل عناصر من المصفوفة",explanation:'المصفوفة مرنة — يمكنك إضافة عناصر جديدة في النهاية بـ "أضف"، أو إدراجها في أي موقع بـ "أدرج". لإزالة عنصر، استخدم "أزل" بالفهرس أو "أزل_آخر". مثل قائمة المشتريات: تضيف وتشطب حسب الحاجة.',code:`متغير قائمة = [1, 2, 3]

# إضافة عنصر في النهاية
أضف(قائمة, 4)
اطبع_سطر("بعد الإضافة: " + قائمة)  # [1, 2, 3, 4]

# إضافة عناصر متعددة
أضف(قائمة, 5)
أضف(قائمة, 6)
اطبع_سطر("القائمة: " + قائمة)

# حذف آخر عنصر
متغير آخر = أزل_آخر(قائمة)
اطبع_سطر("المحذوف: " + آخر)
اطبع_سطر("القائمة: " + قائمة)

# حذف بالفهرس
أزل(قائمة, 2)    # يزيل العنصر ذو الفهرس 2
اطبع_سطر("بعد الحذف: " + قائمة)

# إدراج في موقع محدد
أدرج(قائمة, 0, 99)  # أدرج 99 في الموقع 0
اطبع_سطر("بعد الإدراج: " + قائمة)`,exercise:`# تمرين: ابنِ قائمة مهام:
# 1. ابدأ بمصفوفة فارغة
# 2. أضف 3 مهام
# 3. احذف المهمة الثانية
# 4. أضف مهمة في البداية
# اطبع بعد كل عملية
`,hint:"استخدم أضف وأزل وأدرج",tips:["أضف تضيف في النهاية","أدرج(مصفوفة, موقع, قيمة) تدرج في أي موقع","أزل_آخر ترجع العنصر المحذوف"],concepts:["أضف","أزل","أزل_آخر","أدرج"]},{id:"l2_14",title:"التكرار على المصفوفات",desc:"طرق مختلفة للتكرار على عناصر المصفوفة",explanation:'"لكل" هي الطريقة الأسهل للمرور على كل عنصر — مثل قراءة كتاب صفحة بصفحة. إذا احتجت رقم العنصر (الفهرس)، استخدم "لكل ع في مدى(طول(م))". اختر الطريقة المناسبة حسب ما تحتاجه — البساطة أولاً!',code:`متغير ألوان = ["أحمر", "أخضر", "أزرق", "أصفر"]

# 1. لكل — الأبسط
اطبع_سطر("=== لكل ===")
لكل لون في ألوان
اطبع_سطر("• " + لون)
نهاية

# 2. بالفهرس
اطبع_سطر("\\n=== بالفهرس ===")
لكل ع في مدى(طول(ألوان))
اطبع_سطر(f"{ع + 1}. {ألوان[ع]}")
نهاية

# 3. بينما
اطبع_سطر("\\n=== بينما ===")
متغير ف = 0
بينما (ف < طول(ألوان))
اطبع_سطر(f"  [{ف}] {ألوان[ف]}")
ف += 1
نهاية`,exercise:`# تمرين: لديك مصفوفة درجات [85, 92, 78, 95, 60, 88]
# اطبع كل درجة مع حالتها:
# "88: ناجح ✓" أو "45: راسب ✗"
`,hint:"لكل درجة في الدرجات + إذا (درجة >= 60)",tips:["لكل هي الطريقة المفضلة في أغلب الحالات","استخدم بالفهرس إذا احتجت رقم العنصر","تجنب بينما إلا إذا كنت بحاجة لتحكم دقيق"],concepts:["لكل","تكرار على مصفوفة","فهرسة"]},{id:"l2_15",title:"فرز المصفوفات والعكس",desc:"رتّب واعكس عناصر المصفوفة",explanation:'دالة "رتب" تُرتب العناصر تصاعدياً (من الأصغر للأكبر) — مفيدة لترتيب الدرجات أو الأسماء. دالة "اعكس" تقلب الترتيب بالكامل. للترتيب التنازلي: رتب ثم اعكس! لاحظ أن هذه الدوال تُعدّل المصفوفة الأصلية.',code:`# الفرز (ترتيب تصاعدي)
متغير أرقام = [64, 25, 12, 22, 11]
رتب(أرقام)
اطبع_سطر("مرتبة: " + أرقام)  # [11, 12, 22, 25, 64]

# عكس المصفوفة
اعكس(أرقام)
اطبع_سطر("معكوسة: " + أرقام)  # [64, 25, 22, 12, 11]

# فرز النصوص
متغير أسماء = ["زيد", "أحمد", "ياسر", "بكر"]
رتب(أسماء)
اطبع_سطر("أسماء مرتبة: " + أسماء)

# التحقق من وجود عنصر
متغير فواكه = ["تفاح", "موز", "برتقال"]
اطبع_سطر("هل يحتوي تفاح؟ " + يحتوي(فواكه, "تفاح"))   # صحيح
اطبع_سطر("هل يحتوي كمثرى؟ " + يحتوي(فواكه, "كمثرى")) # خطأ`,exercise:`# تمرين: لديك درجات طلاب
# 1. رتّبها تصاعدياً واطبعها
# 2. اعكسها (تنازلياً) واطبعها
# 3. اطبع أعلى 3 درجات
`,hint:"رتّب ثم اعكس، واستخدم [0], [1], [2] للأعلى 3",tips:["رتب() تعدّل المصفوفة الأصلية","يحتوي() تبحث عن عنصر وترجع صحيح/خطأ"],concepts:["رتب","اعكس","يحتوي","sort","reverse"]},{id:"l2_16",title:"البحث في المصفوفات",desc:"ابحث عن عناصر والبحث بالفهرس والعضوية",explanation:'عامل "في" هو أسرع طريقة للتحقق: "إذا (عنصر في مصفوفة)". لمعرفة موقع العنصر، استخدم "بحث_مصفوفة". الفلترة (استخراج عناصر تطابق شرطاً) تحتاج حلقة ومصفوفة جديدة. البحث مهارة أساسية في التعامل مع البيانات.',code:`متغير طلاب = ["أحمد", "سارة", "محمد", "نورة", "خالد"]

# عامل العضوية: في
إذا ("سارة" في طلاب)
اطبع_سطر("سارة موجودة ✓")
نهاية

إذا (ليس "زيد" في طلاب)
اطبع_سطر("زيد غير موجود ✗")
نهاية

# البحث عن الفهرس
متغير فهرس = بحث_مصفوفة(طلاب, "محمد")
اطبع_سطر(f"محمد في الموقع: {فهرس}")

# فلترة يدوية
متغير أرقام = [15, 8, 22, 3, 41, 7, 19, 30]
متغير كبيرة = []
لكل ر في أرقام
إذا (ر > 20)
    أضف(كبيرة, ر)
نهاية
نهاية
اطبع_سطر("أكبر من 20: " + كبيرة)  # [22, 41, 30]`,exercise:`# تمرين: ابنِ دالة "بحث" تبحث في مصفوفة أسماء
# وترجع كل الأسماء التي تحتوي على حرف معين
`,hint:"كرّر وتحقق بـ يحتوي_على(اسم, حرف)",tips:["في هو عامل العضوية — أبسط طريقة للتحقق","بحث_مصفوفة ترجع الفهرس أو -1","لبناء مصفوفة مفلترة: أنشئ فارغة وأضف ما يطابق الشرط"],concepts:["في","بحث_مصفوفة","عضوية","فلترة"]},{id:"l2_17",title:"الشرائح (Slicing)",desc:"استخرج جزءاً من المصفوفة",explanation:'الشريحة تستخرج "قطعة" من المصفوفة — مثل قص جزء من شريط. "شريحة(م, 2, 5)" تأخذ العناصر من الفهرس 2 إلى 4 (5 غير مشمول). مفيدة لأخذ "أول 3" أو "آخر 5" عناصر، أو لنسخ المصفوفة بالكامل.',code:`متغير أرقام = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90]

# شريحة: من فهرس 2 إلى 5
متغير جزء = شريحة(أرقام, 2, 5)
اطبع_سطر("شريحة [2:5]: " + جزء)  # [20, 30, 40]

# أول 3 عناصر
متغير أول3 = شريحة(أرقام, 0, 3)
اطبع_سطر("أول 3: " + أول3)  # [0, 10, 20]

# آخر 3 عناصر
متغير آخر3 = شريحة(أرقام, طول(أرقام) - 3, طول(أرقام))
اطبع_سطر("آخر 3: " + آخر3)  # [70, 80, 90]

# نسخ المصفوفة
متغير نسخة = شريحة(أرقام, 0, طول(أرقام))
اطبع_سطر("نسخة: " + نسخة)`,exercise:`# تمرين: لديك مصفوفة درجات 10 طلاب
# اطبع: أعلى 3 (بعد الفرز) وأدنى 3
`,hint:"رتّب ثم استخدم الشرائح",tips:["الشريحة تُرجع مصفوفة جديدة ولا تعدّل الأصلية","شريحة(م, بداية, نهاية) — النهاية غير مشمولة"],concepts:["شريحة","slice","استخراج جزء"]},{id:"l2_18",title:"المصفوفات المتداخلة",desc:"مصفوفة داخل مصفوفة — جداول ومصفوفات ثنائية",explanation:'المصفوفة المتداخلة مثل جدول Excel — صفوف وأعمدة. للوصول: "جدول[صف][عمود]". مثالية لبيانات طلاب (كل صف = طالب) أو شبكات الألعاب (X/O). في المستوى الثالث سنرى طريقة أفضل باستخدام الأصناف!',code:`# مصفوفة ثنائية الأبعاد (جدول)
متغير جدول = [
["أحمد", 85, "ناجح"],
["سارة", 92, "ناجح"],
["خالد", 58, "راسب"]
]

# الوصول: جدول[صف][عمود]
اطبع_سطر(f"اسم الطالب الأول: {جدول[0][0]}")   # أحمد
اطبع_سطر(f"درجة سارة: {جدول[1][1]}")           # 92

# طباعة الجدول
لكل صف في جدول
اطبع_سطر(f"  {صف[0]} | {صف[1]} | {صف[2]}")
نهاية

# مصفوفة أرقام 3×3
متغير شبكة = [
[1, 2, 3],
[4, 5, 6],
[7, 8, 9]
]

# مجموع كل صف
لكل ص في مدى(3)
متغير مج = 0
لكل ع في مدى(3)
    مج += شبكة[ص][ع]
نهاية
اطبع_سطر(f"مجموع الصف {ص + 1}: {مج}")
نهاية`,exercise:`# تمرين: أنشئ مصفوفة لجدول مباريات 3 فرق
# كل صف: [الفريق, فوز, تعادل, خسارة, نقاط]
# احسب النقاط (فوز=3, تعادل=1, خسارة=0)
`,hint:"النقاط = فوز * 3 + تعادل * 1",tips:["جدول[ص][ع] = العنصر في الصف ص والعمود ع","المصفوفات المتداخلة مفيدة للبيانات الجدولية"],concepts:["مصفوفة متداخلة","ثنائية الأبعاد","جدول"]},{id:"l2_19",title:"دوال المصفوفات المدمجة",desc:"ملخص كامل لدوال المصفوفات الجاهزة",explanation:'لغة ص توفر دوال جاهزة للمصفوفات: "طول" للحجم، "مجموع" للجمع، "أضف/أزل" للتعديل. نقطة مهمة: المصفوفات تُمرر بالمرجع — أي تعديل داخل دالة يؤثر على الأصلية. لنسخة مستقلة، استخدم "شريحة".',code:`متغير م = [3, 1, 4, 1, 5, 9, 2, 6]

# طول المصفوفة
اطبع_سطر("الطول: " + طول(م))

# الأكبر والأصغر والمجموع
اطبع_سطر("الأكبر: " + أكبر(أكبر(م[0], م[1]), م[2]))
اطبع_سطر("المجموع: " + مجموع(م))

# التحقق من الفراغ
اطبع_سطر("فارغة؟ " + (طول(م) == 0))

# تسطيح مصفوفة متداخلة
متغير متداخلة = [[1, 2], [3, 4], [5, 6]]
اطبع_سطر(متداخلة)

# نسخ المصفوفة (وليس المرجع!)
متغير أصلية = [1, 2, 3]
متغير نسخة = شريحة(أصلية, 0, طول(أصلية))
أضف(نسخة, 99)
اطبع_سطر("الأصلية: " + أصلية)  # [1, 2, 3] — لم تتأثر
اطبع_سطر("النسخة: " + نسخة)    # [1, 2, 3, 99]`,exercise:`# تمرين: اكتب دالة "إحصائيات_مصفوفة(م)" تُرجع:
# المجموع، المتوسط، الأكبر، الأصغر، العدد
`,hint:"احسب كل شيء في حلقة واحدة",tips:["المصفوفات تُمرر بالمرجع — تعديلها داخل دالة يؤثر على الأصلية","استخدم شريحة() لعمل نسخة مستقلة"],concepts:["طول","مجموع","شريحة","نسخ مصفوفة"]},{id:"l2_20",title:"تمرين: إدارة قائمة طلاب",desc:"مشروع تطبيقي — نظام إدارة طلاب بالمصفوفات",explanation:"هذا المشروع يجمع كل ما تعلمته عن المصفوفات في تطبيق عملي. ستستخدم مصفوفتين متوازيتين (الأسماء والدرجات) — نفس الفهرس يشير لنفس الطالب. هذا أساس كل نظام إدارة بيانات!",code:`# نظام إدارة طلاب بسيط
متغير الأسماء = ["أحمد", "سارة", "محمد", "نورة"]
متغير الدرجات = [85, 92, 78, 95]

# دالة عرض الطلاب
دالة عرض()
اطبع_سطر("╔════════════════════════════╗")
اطبع_سطر("║   قائمة الطلاب والدرجات   ║")
اطبع_سطر("╠════════════════════════════╣")
لكل ع في مدى(طول(الأسماء))
    متغير حالة = الدرجات[ع] >= 60 ? "✓" : "✗"
    اطبع_سطر(f"║ {ع + 1}. {الأسماء[ع]}: {الدرجات[ع]} {حالة}  ║")
نهاية
اطبع_سطر("╚════════════════════════════╝")
نهاية

# دالة المتوسط
دالة المتوسط()
ارجع مجموع(الدرجات) / طول(الدرجات)
نهاية

عرض()
اطبع_سطر(f"المتوسط العام: {المتوسط()}")`,exercise:`# تمرين: أضف للنظام:
# 1. دالة إضافة_طالب(اسم, درجة)
# 2. دالة أفضل_طالب() — ترجع اسم صاحب أعلى درجة
# 3. دالة الراسبين() — ترجع مصفوفة الراسبين
`,hint:"أضف للمصفوفتين معاً، وابحث عن الأعلى بحلقة",tips:["مصفوفتان متوازيتان: نفس الفهرس = نفس الطالب","في المستوى 3 سنتعلم الأصناف — طريقة أفضل لتنظيم البيانات"],concepts:["مشروع تطبيقي","مصفوفات متوازية"]}]},oc={id:"l2_s3",title:"النصوص المتقدمة",icon:"🔤",lessons:[{id:"l2_21",title:"تقسيم ودمج النصوص",desc:"حوّل بين النصوص والمصفوفات",explanation:'"قسم" تفكك نصاً إلى مصفوفة كلمات — مثل تقطيع الكيكة إلى قطع. "صل" تفعل العكس — تجمع المصفوفة في نص واحد بفاصل بينها. هاتان الدالتان أساسيتان للتعامل مع ملفات CSV والبيانات المنظمة.',code:`# تقسيم نص إلى مصفوفة
متغير جملة = "أحب البرمجة بلغة ص"
متغير كلمات = قسم(جملة, " ")
اطبع_سطر(كلمات)  # ["أحب", "البرمجة", "بلغة", "ص"]

# دمج مصفوفة في نص
متغير مدمج = صل(كلمات, " - ")
اطبع_سطر(مدمج)  # أحب - البرمجة - بلغة - ص

# تقسيم بفاصل آخر
متغير csv = "أحمد,25,الرياض,مبرمج"
متغير حقول = قسم(csv, ",")
لكل حقل في حقول
اطبع_سطر("  → " + حقل)
نهاية

# تقسيم بسطور جديدة
متغير نص_أسطر = "سطر1\\nسطر2\\nسطر3"
متغير أسطر = قسم(نص_أسطر, "\\n")
اطبع_سطر("عدد الأسطر: " + طول(أسطر))`,exercise:`# تمرين: لديك CSV ببيانات طلاب:
# "أحمد:85,سارة:92,محمد:78"
# قسّمه واستخرج كل اسم ودرجة
`,hint:"قسم بالفاصلة ، ثم قسم كل جزء بالنقطتين :",tips:["قسم + صل عمليتان عكسيتان","مفيدة جداً لمعالجة ملفات CSV والبيانات المنظمة"],concepts:["قسم","صل","split","join"]},{id:"l2_22",title:"البحث والاستبدال في النصوص",desc:"ابحث عن نمط واستبدله بنص آخر",explanation:'"بحث" تجد موقع كلمة في النص، و"استبدل" تغيّرها بكلمة أخرى — مثل خاصية "Find & Replace" في Word. مفيدة جداً لبناء قوالب رسائل أو تنظيف بيانات. يمكنك تسلسل عدة استبدالات لتغييرات متعددة.',code:`متغير نص = "مرحبا بك في عالم البرمجة. البرمجة ممتعة!"

# البحث
اطبع_سطر("يحتوي 'برمجة'؟ " + يحتوي_على(نص, "برمجة"))   # صحيح
اطبع_سطر("موقع 'عالم': " + بحث(نص, "عالم"))

# الاستبدال
متغير جديد = استبدل(نص, "البرمجة", "التقنية")
اطبع_سطر(جديد)

# استبدال متعدد
متغير قالب = "مرحبا {اسم}, عمرك {عمر} سنة"
متغير نتيجة = استبدل(قالب, "{اسم}", "أحمد")
نتيجة = استبدل(نتيجة, "{عمر}", "25")
اطبع_سطر(نتيجة)

# عدّ التكرارات
متغير جملة = "أحب البرمجة والبرمجة تحبني"
متغير عدد = 0
متغير موقع = بحث(جملة, "البرمجة")
اطبع_سطر(f"أول ظهور للبرمجة: {موقع}")`,exercise:`# تمرين: اكتب دالة "رقابة" تستبدل كلمات معينة بنجوم
# مثال: رقابة("هذا نص سيء جداً", ["سيء"]) → "هذا نص *** جداً"
`,hint:"كرّر على كلمات الرقابة واستبدل كلاً منها",tips:["استبدل يستبدل أول ظهور فقط في بعض التطبيقات","يمكن تسلسل عمليات الاستبدال"],concepts:["بحث","استبدل","يحتوي_على"]},{id:"l2_23",title:"التحقق من خصائص النص",desc:"فحص النص: يبدأ/ينتهي/يحتوي/نوعه",explanation:'دوال مثل "يبدأ_ب" و"ينتهي_ب" و"يحتوي_على" تفحص النص وتُرجع صحيح/خطأ. أساسية للتحقق من المدخلات — هل البريد يحتوي @؟ هل الهاتف يبدأ بـ 05؟ التحقق من المدخلات ممارسة ضرورية لكل مبرمج.',code:`متغير بريد = "user@example.com"
متغير هاتف = "0501234567"
متغير اسم = "أحمد محمد"

# يبدأ بـ / ينتهي بـ
اطبع_سطر(ينتهي_ب(بريد, ".com"))     # صحيح
اطبع_سطر(يبدأ_ب(هاتف, "05"))        # صحيح

# التحقق من المحتوى
اطبع_سطر(يحتوي_على(بريد, "@"))      # صحيح

# دالة تحقق بسيطة
دالة تحقق_بريد(البريد)
إذا (يحتوي_على(البريد, "@") و ينتهي_ب(البريد, ".com"))
    ارجع صحيح
نهاية
ارجع خطأ
نهاية

اطبع_سطر(f"بريد صحيح؟ {تحقق_بريد(بريد)}")

# التحقق من الطول
دالة تحقق_كلمة_سر(كلمة)
إذا (طول(كلمة) < 8)
    اطبع_سطر("كلمة السر قصيرة جداً!")
    ارجع خطأ
نهاية
اطبع_سطر("كلمة سر مقبولة ✓")
ارجع صحيح
نهاية

تحقق_كلمة_سر("abc")
تحقق_كلمة_سر("strongPass123")`,exercise:`# تمرين: اكتب دالة "تحقق_هاتف" تتحقق:
# 1. يبدأ بـ "05"
# 2. طوله 10 أرقام
# 3. يحتوي فقط على أرقام
`,hint:"استخدم يبدأ_ب و طول للفحصين الأولين",tips:["التحقق من المدخلات ممارسة أساسية","ارجع مبكراً عند فشل أي تحقق"],concepts:["يبدأ_ب","ينتهي_ب","يحتوي_على","تحقق"]},{id:"l2_24",title:"قص الأطراف والمحاذاة",desc:"نظّف المسافات وحاذِ النصوص",explanation:'"قص_أطراف" تزيل المسافات الزائدة من بداية ونهاية النص — ضرورية عند التعامل مع إدخال المستخدم. غالباً المستخدم يضغط مسافة بالخطأ في النهاية، و"قص_أطراف" تعالج ذلك تلقائياً. التنسيق الجيد يجعل مخرجاتك احترافية.',code:`# قص المسافات من الأطراف
متغير مدخل = "   مرحبا   "
اطبع_سطر(f"قبل القص: '{مدخل}'")
اطبع_سطر(f"بعد القص: '{قص_أطراف(مدخل)}'")

# تنظيف مدخلات المستخدم
دالة نظف_اسم(اسم)
متغير نظيف = قص_أطراف(اسم)
ارجع نظيف
نهاية

اطبع_سطر(نظف_اسم("  أحمد  "))  # "أحمد"

# تكرار نص
متغير خط = "═"
متغير شريط = ""
لكل ع في مدى(30)
شريط += خط
نهاية
اطبع_سطر(شريط)

# تنسيق جدول بسيط
متغير بيانات = [["أحمد", "85"], ["سارة", "92"], ["محمد", "78"]]
لكل صف في بيانات
اطبع_سطر(f"| {صف[0]} | {صف[1]} |")
نهاية`,exercise:`# تمرين: اكتب دالة "تنسيق_جدول" تستقبل مصفوفة ثنائية
# وتطبعها في جدول منسق بحدود
`,hint:"احسب أطول عنصر في كل عمود ثم حاذِ",tips:["قص_أطراف مهمة جداً للتعامل مع إدخال المستخدم","التنسيق يحسّن قراءة المخرجات"],concepts:["قص_أطراف","trim","تنسيق"]},{id:"l2_25",title:"دعم Unicode والعربية",desc:"لغة ص تدعم Unicode بالكامل — تعامل مع النصوص العربية",explanation:"لغة ص صُممت لدعم العربية بالكامل — أسماء المتغيرات والدوال والنصوص كلها تعمل بالعربية. حتى رموز Emoji مدعومة! هذا يعني أنك تكتب كوداً طبيعياً بلغتك بدون قيود.",code:`# النصوص العربية تعمل بالكامل
متغير تحية = "مرحبا بالعالم العربي!"
اطبع_سطر("النص: " + تحية)
اطبع_سطر("الطول: " + طول(تحية))

# أسماء متغيرات عربية بالكامل
متغير عدد_الطلاب = 42
متغير نسبة_النجاح = 95.5
اطبع_سطر(f"الطلاب: {عدد_الطلاب}, النجاح: {نسبة_النجاح}%")

# رموز ايموجي تعمل أيضاً
متغير رسالة = "البرمجة ممتعة! 🎉🚀✨"
اطبع_سطر(رسالة)

# مزج العربية والإنجليزية
متغير معلومات = "الإصدار: v2.0 — الحالة: stable"
اطبع_سطر(معلومات)`,exercise:`# تمرين: اطبع جملة عربية كاملة
# ثم اطبع كل حرف على سطر مع رقمه
`,hint:"استخدم لكل مع عداد أو فهرس",tips:["لغة ص تدعم UTF-8 بالكامل","الأسماء العربية في المتغيرات والدوال مدعومة 100%","Emoji والرموز الخاصة تعمل في النصوص"],concepts:["UTF-8","Unicode","عربية"]},{id:"l2_26",title:"دوال النصوص المدمجة — مرجع",desc:"ملخص شامل لكل دوال النصوص المتاحة",explanation:"هذا مرجع سريع لأهم دوال النصوص: طول، يحتوي_على، استبدل، قسم، صل، قص_أطراف. احفظ هذه الدوال جيداً — ستستخدمها يومياً في كل برنامج يتعامل مع النصوص. التعامل مع النصوص أساس 80% من البرمجة!",code:`متغير نص = "  مرحبا بالعالم العربي  "

# القص والتنظيف
اطبع_سطر(قص_أطراف(نص))              # "مرحبا بالعالم العربي"

# الطول
اطبع_سطر("الطول: " + طول(قص_أطراف(نص)))

# البحث
اطبع_سطر(يحتوي_على(نص, "عالم"))     # صحيح
اطبع_سطر(يبدأ_ب(قص_أطراف(نص), "مرحبا")) # صحيح
اطبع_سطر(ينتهي_ب(قص_أطراف(نص), "العربي")) # صحيح

# الاستخراج
اطبع_سطر(استخراج(قص_أطراف(نص), 0, 5))  # "مرحبا"

# التحويل
متغير eng = "Hello World"
اطبع_سطر(أحرف_كبيرة(eng))  # "HELLO WORLD"
اطبع_سطر(أحرف_صغيرة(eng))  # "hello world"

# التقسيم والدمج
متغير أجزاء = قسم(قص_أطراف(نص), " ")
اطبع_سطر("الكلمات: " + أجزاء)
اطبع_سطر("مدمجة: " + صل(أجزاء, "-"))

# الاستبدال
اطبع_سطر(استبدل(نص, "العربي", "الرقمي"))`,exercise:`# تمرين: اكتب دالة "عكس_نص" تعكس ترتيب أحرف النص
# مثال: "مرحبا" → "ابحرم"
`,hint:"حوّل لمصفوفة حروف، اعكسها، ثم ادمجها",tips:["هذه الدوال أساسية ومستخدمة يومياً","احفظ الأهم: طول، يحتوي_على، استبدل، قسم، صل"],concepts:["مرجع دوال النصوص"]},{id:"l2_27",title:"تنسيق النصوص المتقدم",desc:"بناء قوالب نصوص وتنسيق متقدم",explanation:'القوالب (Templates) نصوص جاهزة فيها "فراغات" تملؤها ببيانات. f-string مثالية لبناء رسائل ترحيب، شهادات، تقارير منسقة. استخدم f"""..""" للنصوص متعددة الأسطر لتنسيقات معقدة.',code:`# قالب رسالة
دالة رسالة_ترحيب(الاسم, المنصب, الشركة)
ارجع f"""
╔══════════════════════════════════════╗
║                                      ║
║  مرحبا {الاسم}!                     ║
║  منصبك: {المنصب}                    ║
║  الشركة: {الشركة}                   ║
║                                      ║
╚══════════════════════════════════════╝"""
نهاية

اطبع_سطر(رسالة_ترحيب("أحمد", "مبرمج", "تقنية"))

# بناء HTML بسيط
دالة عنصر_html(الوسم, المحتوى)
ارجع f"<{الوسم}>{المحتوى}</{الوسم}>"
نهاية

اطبع_سطر(عنصر_html("h1", "عنوان"))
اطبع_سطر(عنصر_html("p", "فقرة"))`,exercise:`# تمرين: اكتب دالة "شهادة_تخرج" تطبع شهادة منسقة
# تتضمن: الاسم، التخصص، المعدل، التاريخ
`,hint:"استخدم f-string مع أسطر متعددة",tips:['f"..." للأسطر الفردية','f"""...""" لنصوص متعددة الأسطر'],concepts:["قوالب نصوص","f-string متقدم"]},{id:"l2_28",title:"تمرين: محلل نصوص",desc:"ابنِ أداة تحلل خصائص النص",explanation:"هذا المشروع يجمع كل دوال النصوص لبناء أداة تحليل حقيقية. ستحسب عدد الكلمات والحروف والأسطر، وتجد أطول كلمة. تحليل النصوص أساس كثير من التطبيقات مثل محركات البحث.",code:`# محلل نصوص بسيط
دالة حلل_نص(النص)
متغير كلمات = قسم(النص, " ")
متغير عدد_كلمات = طول(كلمات)
متغير عدد_حروف = طول(النص)
متغير عدد_أسطر = طول(قسم(النص, "\\n"))

اطبع_سطر("═══ تحليل النص ═══")
اطبع_سطر(f"عدد الحروف: {عدد_حروف}")
اطبع_سطر(f"عدد الكلمات: {عدد_كلمات}")
اطبع_سطر(f"عدد الأسطر: {عدد_أسطر}")
اطبع_سطر(f"متوسط طول الكلمة: {عدد_حروف / عدد_كلمات}")

# أطول كلمة
متغير أطول = كلمات[0]
لكل كلمة في كلمات
    إذا (طول(كلمة) > طول(أطول))
        أطول = كلمة
    نهاية
نهاية
اطبع_سطر(f"أطول كلمة: {أطول} ({طول(أطول)} حرف)")
نهاية

حلل_نص("لغة ص هي لغة برمجة عربية حديثة وممتعة")`,exercise:`# تمرين: أضف للمحلل:
# 1. عدّاد كلمات (كم مرة تكررت كل كلمة)
# 2. أقصر كلمة
# 3. الكلمة الأكثر تكراراً
`,hint:"استخدم حلقة لكل مع متغيرات تتبع",tips:["تحليل النصوص أساس كثير من التطبيقات","سنتعلم الخرائط لاحقاً — أسهل لعدّ الكلمات"],concepts:["تحليل نصوص","مشروع تطبيقي"]}]},ic={id:"l2_s4",title:"الخرائط والقواميس",icon:"🗺️",lessons:[{id:"l2_29",title:"إنشاء الخرائط",desc:"الخريطة تخزّن أزواج مفتاح-قيمة",explanation:'الخريطة مثل قاموس اللغة — كل كلمة (مفتاح) لها معنى (قيمة). بدلاً من الوصول برقم (كالمصفوفة)، تصل بالاسم: طالب["العمر"]. مثالية لتخزين بيانات ذات خصائص متعددة كمعلومات شخص أو إعدادات برنامج.',code:`# إنشاء خريطة
متغير طالب = {
"الاسم": "أحمد",
"العمر": 20,
"المدينة": "الرياض",
"نشط": صحيح
}

اطبع_سطر(طالب)

# الوصول للقيم
اطبع_سطر("الاسم: " + طالب["الاسم"])
اطبع_سطر("العمر: " + طالب["العمر"])

# خريطة فارغة
متغير إعدادات = {}
اطبع_سطر("فارغة: " + إعدادات)`,exercise:`# تمرين: أنشئ خريطة تصف كتاباً:
# العنوان، المؤلف، السنة، الناشر، عدد الصفحات
`,hint:'متغير كتاب = {"العنوان": "...", ...}',tips:["المفاتيح عادةً نصوص","القيم يمكن أن تكون أي نوع","الخرائط تُسمى أيضاً: قواميس، كائنات، hash maps"],concepts:["خريطة","مفتاح","قيمة","map"]},{id:"l2_30",title:"الوصول والتعديل",desc:"اقرأ وأضف وحدّث واحذف عناصر الخريطة",explanation:'الخريطة مرنة جداً: خريطة["مفتاح"] = قيمة تُضيف أو تُحدّث حسب وجود المفتاح. "(مفتاح في خريطة)" تتحقق من الوجود. "احذف" تزيل زوج مفتاح-قيمة. هذه العمليات الأربع (إضافة، قراءة، تحديث، حذف) تُسمى CRUD.',code:`متغير شخص = {"الاسم": "سارة", "العمر": 25}

# الوصول (القراءة)
اطبع_سطر("الاسم: " + شخص["الاسم"])

# الإضافة (مفتاح جديد)
شخص["المهنة"] = "مهندسة"
شخص["المدينة"] = "جدة"
اطبع_سطر("بعد الإضافة: " + شخص)

# التحديث (مفتاح موجود)
شخص["العمر"] = 26
اطبع_سطر("بعد التحديث: " + شخص)

# الحذف
احذف(شخص, "المدينة")
اطبع_سطر("بعد الحذف: " + شخص)

# التحقق من وجود مفتاح
إذا ("المهنة" في شخص)
اطبع_سطر("المهنة موجودة: " + شخص["المهنة"])
نهاية`,exercise:`# تمرين: ابنِ قاموس إنجليزي-عربي بسيط
# أضف 5 كلمات ثم ابحث عن ترجمة كلمة معينة
`,hint:'متغير قاموس = {"hello": "مرحبا", ...}',tips:['خريطة["مفتاح"] = قيمة → يُضيف أو يُحدّث',"في تتحقق من وجود مفتاح","احذف تزيل زوج مفتاح-قيمة"],concepts:["إضافة","تحديث","حذف","في"]},{id:"l2_31",title:"التكرار على خريطة",desc:"طرق المرور على أزواج المفتاح-القيمة",explanation:'"مفاتيح(خريطة)" تُرجع مصفوفة المفاتيح، و"قيم(خريطة)" تُرجع مصفوفة القيم. للمرور على كليهما: كرر على المفاتيح واستخدمها للوصول للقيم. لاحظ أن الخرائط لا تحفظ الترتيب بالضرورة.',code:`متغير درجات = {
"أحمد": 85,
"سارة": 92,
"محمد": 78,
"نورة": 95
}

# التكرار على المفاتيح
اطبع_سطر("=== الأسماء ===")
لكل اسم في مفاتيح(درجات)
اطبع_سطر("• " + اسم)
نهاية

# التكرار على القيم
اطبع_سطر("\\n=== الدرجات ===")
لكل درجة في قيم(درجات)
اطبع_سطر("• " + درجة)
نهاية

# التكرار على كليهما
اطبع_سطر("\\n=== التقرير ===")
لكل اسم في مفاتيح(درجات)
متغير حالة = درجات[اسم] >= 80 ? "ممتاز" : "جيد"
اطبع_سطر(f"{اسم}: {درجات[اسم]} ({حالة})")
نهاية`,exercise:`# تمرين: لديك خريطة أسعار منتجات
# اطبع المنتجات الأغلى من 50 ريال فقط
`,hint:"كرّر على المفاتيح وتحقق من القيمة",tips:["مفاتيح() ترجع مصفوفة المفاتيح","قيم() ترجع مصفوفة القيم","الخرائط لا تحفظ ترتيب الإدراج بالضرورة"],concepts:["مفاتيح","قيم","تكرار على خريطة"]},{id:"l2_32",title:"التحقق من المفاتيح",desc:"تحقق من وجود مفتاح قبل الوصول إليه",explanation:'محاولة الوصول لمفتاح غير موجود تسبب خطأ! لذا تحقق أولاً بـ "(مفتاح في خريطة)". هذا نمط "البرمجة الدفاعية" — احمِ برنامجك من الأخطاء قبل حدوثها. يمكنك أيضاً تعيين قيمة افتراضية إذا لم يوجد المفتاح.',code:`متغير إعدادات = {
"اللغة": "العربية",
"الثيم": "داكن",
"حجم_الخط": 14
}

# التحقق بـ في
إذا ("اللغة" في إعدادات)
اطبع_سطر("اللغة: " + إعدادات["اللغة"])
نهاية

# التحقق قبل الوصول — لمنع الأخطاء
متغير مفتاح = "اللون"
إذا (مفتاح في إعدادات)
اطبع_سطر("القيمة: " + إعدادات[مفتاح])
وإلا
اطبع_سطر(f"المفتاح '{مفتاح}' غير موجود")
# إضافة قيمة افتراضية
إعدادات[مفتاح] = "أزرق"
اطبع_سطر(f"تم تعيين '{مفتاح}' = 'أزرق'")
نهاية

اطبع_سطر(إعدادات)`,exercise:`# تمرين: اكتب دالة "إعداد_آمن" تستقبل خريطة ومفتاح وقيمة افتراضية
# ترجع القيمة إذا وُجد المفتاح، وإلا ترجع الافتراضية
`,hint:"تحقق بـ في ثم ارجع القيمة أو الافتراضية",tips:["دائماً تحقق من وجود المفتاح قبل الوصول",'هذا النمط يُعرف بـ "الحماية من الأخطاء" (defensive programming)'],concepts:["في","تحقق من مفتاح","قيمة افتراضية"]},{id:"l2_33",title:"خرائط متداخلة",desc:"خريطة داخل خريطة — بيانات معقدة",explanation:'الخرائط المتداخلة تمثل بيانات هرمية — مثل ملف طالب فيه عنوان (والعنوان فيه مدينة وحي) ودرجات (والدرجات فيها مواد مختلفة). للوصول: طالب["عنوان"]["مدينة"]. هيكلة قوية للبيانات المعقدة!',code:`# خريطة متداخلة — بيانات طالب كاملة
متغير طالب = {
"الاسم": "أحمد",
"العمر": 20,
"العنوان": {
    "المدينة": "الرياض",
    "الحي": "العليا",
    "الرمز": "12345"
},
"الدرجات": {
    "رياضيات": 95,
    "فيزياء": 88,
    "كيمياء": 92
}
}

# الوصول للبيانات المتداخلة
اطبع_سطر("المدينة: " + طالب["العنوان"]["المدينة"])
اطبع_سطر("درجة الرياضيات: " + طالب["الدرجات"]["رياضيات"])

# التعديل
طالب["الدرجات"]["فيزياء"] = 90
اطبع_سطر(طالب["الدرجات"])`,exercise:`# تمرين: أنشئ خريطة لمتجر تحتوي:
# - اسم المتجر
# - الموقع (مدينة، شارع)
# - المنتجات (اسم → سعر) لـ 3 منتجات
# اطبع كل المعلومات بتنسيق جميل
`,hint:'خريطة داخل خريطة: {"المنتجات": {"قلم": 5, ...}}',tips:["الخرائط المتداخلة مثالية للبيانات الهرمية","في المستوى 3 سنتعلم الأصناف — بديل أفضل"],concepts:["خرائط متداخلة","بيانات هرمية"]},{id:"l2_34",title:"الخرائط والمصفوفات معاً",desc:"ادمج الخرائط والمصفوفات لبناء بيانات غنية",explanation:"مصفوفة من الخرائط = قاعدة بيانات بسيطة! كل خريطة = سجل واحد (طالب، منتج...) والمصفوفة = الجدول الكامل. يمكنك البحث، الفلترة، حساب المتوسطات بسهولة. هذا أساس كل تطبيق إدارة بيانات!",code:`# مصفوفة خرائط — قاعدة بيانات بسيطة
متغير طلاب = [
{"الاسم": "أحمد", "العمر": 20, "درجة": 85},
{"الاسم": "سارة", "العمر": 22, "درجة": 92},
{"الاسم": "محمد", "العمر": 21, "درجة": 78},
{"الاسم": "نورة", "العمر": 20, "درجة": 95}
]

# البحث عن طالب
لكل طالب في طلاب
إذا (طالب["الاسم"] == "سارة")
    اطبع_سطر(f"وجدت: {طالب}")
نهاية
نهاية

# فلترة: الناجحون فقط
متغير ناجحون = []
لكل طالب في طلاب
إذا (طالب["درجة"] >= 80)
    أضف(ناجحون, طالب["الاسم"])
نهاية
نهاية
اطبع_سطر("الناجحون بامتياز: " + ناجحون)

# حساب المتوسط
متغير مجموع_درجات = 0
لكل طالب في طلاب
مجموع_درجات += طالب["درجة"]
نهاية
اطبع_سطر(f"المتوسط: {مجموع_درجات / طول(طلاب)}")`,exercise:`# تمرين: أنشئ "قاعدة بيانات" منتجات (مصفوفة خرائط)
# كل منتج: اسم، سعر، كمية
# اكتب دالة بحث(اسم) ودالة إجمالي_المخزون()
`,hint:"كرّر على المنتجات وقارن الأسماء",tips:["مصفوفة الخرائط = أبسط شكل لقاعدة بيانات","كل خريطة = سجل واحد (record)","المصفوفة = جدول البيانات (table)"],concepts:["مصفوفة خرائط","قاعدة بيانات بسيطة"]},{id:"l2_35",title:"تمرين: دفتر عناوين",desc:"ابنِ دفتر عناوين تفاعلي",explanation:"مشروع عملي يجمع الخرائط والمصفوفات والدوال. ستبني نظام إدارة جهات اتصال بعمليات CRUD (إضافة، عرض، بحث، حذف). هذا النمط أساس كل تطبيق إدارة بيانات حقيقي!",code:`# دفتر عناوين بسيط
متغير جهات_الاتصال = [
{"الاسم": "أحمد", "الهاتف": "0501234567", "البريد": "ahmed@mail.com"},
{"الاسم": "سارة", "الهاتف": "0559876543", "البريد": "sara@mail.com"}
]

دالة عرض_الكل()
اطبع_سطر("═══ دفتر العناوين ═══")
لكل ع في مدى(طول(جهات_الاتصال))
    متغير ج = جهات_الاتصال[ع]
    اطبع_سطر(f"{ع + 1}. {ج['الاسم']} | ☎ {ج['الهاتف']} | ✉ {ج['البريد']}")
نهاية
نهاية

دالة أضف_جهة(الاسم, الهاتف, البريد)
أضف(جهات_الاتصال, {
    "الاسم": الاسم,
    "الهاتف": الهاتف,
    "البريد": البريد
})
اطبع_سطر(f"✓ تمت إضافة {الاسم}")
نهاية

دالة ابحث(اسم)
لكل ج في جهات_الاتصال
    إذا (يحتوي_على(ج["الاسم"], اسم))
        اطبع_سطر(f"وجدت: {ج['الاسم']} — {ج['الهاتف']}")
        ارجع ج
    نهاية
نهاية
اطبع_سطر("لم يُعثر على " + اسم)
ارجع لاشيء
نهاية

أضف_جهة("محمد", "0512345678", "moh@mail.com")
عرض_الكل()
ابحث("سار")`,exercise:`# تمرين: أضف للدفتر:
# 1. دالة حذف_جهة(اسم)
# 2. دالة تحديث_هاتف(اسم, هاتف_جديد)
# 3. دالة تصدير() — تطبع الكل بصيغة CSV
`,hint:"للحذف: ابحث عن الفهرس ثم أزل. للتحديث: ابحث وعدّل",tips:["هذا النمط هو أساس كل تطبيقات إدارة البيانات","CRUD: Create, Read, Update, Delete"],concepts:["CRUD","تطبيق عملي","دفتر عناوين"]},{id:"l2_36",title:"تمرين: عدّاد كلمات",desc:"حلّل نصاً وأحصِ تكرار كل كلمة",explanation:"عدّاد الكلمات نمط شائع جداً — الخريطة مثالية له: الكلمة = مفتاح، عدد المرات = قيمة. يُستخدم في محركات البحث، تحليل النصوص، وحتى كشف اللغة. تدريب ممتاز على دمج الأدوات!",code:`# عدّاد كلمات
دالة عدّ_كلمات(النص)
متغير كلمات = قسم(أحرف_صغيرة(قص_أطراف(النص)), " ")
متغير عداد = {}

لكل كلمة في كلمات
    إذا (كلمة في عداد)
        عداد[كلمة] += 1
    وإلا
        عداد[كلمة] = 1
    نهاية
نهاية

ارجع عداد
نهاية

متغير نص = "أحب البرمجة والبرمجة تحب من يحب البرمجة"
متغير النتيجة = عدّ_كلمات(نص)

اطبع_سطر("═══ تكرار الكلمات ═══")
لكل كلمة في مفاتيح(النتيجة)
اطبع_سطر(f"  {كلمة}: {النتيجة[كلمة]} مرة")
نهاية`,exercise:`# تمرين: وسّع عدّاد الكلمات ليشمل:
# 1. الكلمة الأكثر تكراراً
# 2. الكلمات التي ظهرت مرة واحدة فقط
# 3. ترتيب الكلمات حسب تكرارها
`,hint:"كرّر على النتيجة وابحث عن الأعلى",tips:["عدّاد الكلمات نمط شائع جداً — يُستخدم في محركات البحث","الخريطة مثالية لتتبع التكرارات"],concepts:["عدّاد كلمات","تحليل نصوص","خريطة"]}]},ac={id:"l2_s5",title:"معالجة الأخطاء",icon:"🛡️",lessons:[{id:"l2_37",title:"فهم الأخطاء",desc:"لماذا تحدث الأخطاء وكيف نتعامل معها",explanation:"الأخطاء جزء طبيعي من البرمجة — كل مبرمج يواجهها! هناك 3 أنواع: نحوية (كأخطاء الكتابة)، وقت التشغيل (كالقسمة على صفر)، ومنطقية (البرنامج يعمل لكن النتيجة خاطئة). اقرأ رسائل الخطأ بعناية — عادة تخبرك أين المشكلة!",code:`# أنواع الأخطاء الشائعة:

# 1. خطأ نحوي (Syntax Error) — يمنع التشغيل
# مثال: نسيان كلمة نهاية

# 2. خطأ وقت التشغيل (Runtime Error)
# مثال: القسمة على صفر
# متغير ن = 10 / 0   # ❌

# 3. خطأ منطقي (Logic Error) — النتيجة خاطئة
# مثال: جمع بدل ضرب
# متغير مساحة = الطول + العرض  # ❌ يجب ×

# التعامل الآمن — بدون try/catch
دالة قسمة_آمنة(أ, ب)
إذا (ب == 0)
    اطبع_سطر("خطأ: لا يمكن القسمة على صفر!")
    ارجع لاشيء
نهاية
ارجع أ / ب
نهاية

اطبع_سطر(قسمة_آمنة(10, 3))
اطبع_سطر(قسمة_آمنة(10, 0))`,exercise:`# تمرين: اكتب 3 أمثلة لأخطاء شائعة (واحد من كل نوع)
# ثم أصلحها
`,hint:"خطأ نحوي: نسيان نهاية. وقت تشغيل: فهرس خارج الحدود. منطقي: عملية خاطئة",tips:["الأخطاء طبيعية — كل مبرمج يواجهها","اقرأ رسالة الخطأ بعناية — عادة تخبرك أين المشكلة","الأخطاء المنطقية الأصعب — لا تظهر رسالة خطأ!"],concepts:["أنواع الأخطاء","syntax error","runtime error"]},{id:"l2_38",title:"حاول / امسك (try/catch)",desc:"التقط الأخطاء وتعامل معها بدل توقف البرنامج",explanation:'"حاول" تحمي الكود الذي قد يفشل، و"امسك" تلتقط الخطأ إذا حدث — مثل شبكة الأمان للبهلوان. بدلاً من توقف البرنامج، يمكنك عرض رسالة لطيفة للمستخدم أو تجربة بديل. لا تستخدمها لتدفق البرنامج العادي.',code:`# حاول / امسك — التعامل مع أخطاء وقت التشغيل
حاول
متغير نتيجة = 10 / 0
اطبع_سطر("النتيجة: " + نتيجة)
امسك (خطأ)
اطبع_سطر("حدث خطأ: " + خطأ)
نهاية

اطبع_سطر("البرنامج مستمر!") # لم يتوقف

# مثال عملي: تحويل آمن
دالة لرقم_آمن(نص)
حاول
    ارجع لرقم(نص)
امسك (خطأ)
    اطبع_سطر(f"'{نص}' ليس رقماً صحيحاً")
    ارجع 0
نهاية
نهاية

اطبع_سطر(لرقم_آمن("42"))    # 42
اطبع_سطر(لرقم_آمن("abc"))   # 0`,exercise:`# تمرين: اكتب دالة "وصول_آمن" تصل لعنصر مصفوفة
# بدون توقف البرنامج إذا كان الفهرس خارج الحدود
`,hint:"حاول ... امسك ثم ارجع قيمة افتراضية",tips:["حاول تحمي الكود الذي قد يفشل","امسك تلتقط الخطأ وتوفر بديلاً","لا تستخدم try/catch لتدفق البرنامج العادي"],concepts:["حاول","امسك","try","catch"]},{id:"l2_39",title:"كتلة أخيراً (finally)",desc:"كود يُنفَّذ دائماً — سواء حدث خطأ أم لا",explanation:'"أخيراً" تُنفذ دائماً — سواء نجح الكود أو فشل. مثالية لتنظيف الموارد: إغلاق ملفات، إنهاء اتصالات. فكّر بها كأمر "بغض النظر عما يحدث، افعل هذا في النهاية". اختيارية — يمكنك استخدام حاول/امسك بدونها.',code:`# أخيراً — تُنفذ دائماً
اطبع_سطر("=== اختبار أخيراً ===")

حاول
اطبع_سطر("1. محاولة العملية...")
متغير ن = 10 / 0
اطبع_سطر("2. لن تُطبع هذه")
امسك (خطأ)
اطبع_سطر("3. التقطنا الخطأ: " + خطأ)
أخيراً
اطبع_سطر("4. هذه تُنفذ دائماً!")
نهاية

# استخدام عملي: تنظيف الموارد
دالة اقرأ_بيانات()
اطبع_سطر("فتح الملف...")
حاول
    # محاولة قراءة بيانات
    اطبع_سطر("قراءة المحتوى...")
    # قد يحدث خطأ هنا
امسك (خطأ)
    اطبع_سطر("فشلت القراءة: " + خطأ)
أخيراً
    اطبع_سطر("إغلاق الملف ✓")  # ✓ يُنفذ دائماً
نهاية
نهاية

اقرأ_بيانات()`,exercise:`# تمرين: اكتب دالة تحاول الاتصال بخادم (محاكاة)
# استخدم أخيراً لطباعة "تم إغلاق الاتصال" دائماً
`,hint:"ضع منطق الاتصال في حاول وإغلاق الاتصال في أخيراً",tips:["أخيراً لتنظيف الموارد: إغلاق ملفات، اتصالات...","تُنفذ سواء نجح أو فشل الكود في حاول","أخيراً اختيارية — يمكن استخدام حاول/امسك فقط"],concepts:["أخيراً","finally","تنظيف الموارد"]},{id:"l2_40",title:"رمي الاستثناءات (throw)",desc:"أنشئ وارمِ أخطاءك الخاصة",explanation:'"ارمِ" تجعل دالتك تُرسل خطأ عندما تكتشف مشكلة — مثل رفع العلم الأحمر. المستدعي يمسك الخطأ بـ حاول/امسك. استخدمها للتحقق من المدخلات الخاطئة. رسالة الخطأ يجب أن تكون واضحة ومحددة!',code:`# رمي خطأ مخصص
دالة سحب(الرصيد, المبلغ)
إذا (المبلغ <= 0)
    ارمي "المبلغ يجب أن يكون موجباً!"
نهاية
إذا (المبلغ > الرصيد)
    ارمي "رصيد غير كافٍ! المتاح: " + الرصيد
نهاية
ارجع الرصيد - المبلغ
نهاية

# التعامل مع الخطأ المرمي
حاول
متغير رصيد = 1000
رصيد = سحب(رصيد, 500)
اطبع_سطر("الرصيد المتبقي: " + رصيد)

رصيد = سحب(رصيد, 600)  # سيرمي خطأ!
امسك (خطأ)
اطبع_سطر("⚠️ " + خطأ)
نهاية

# تحقق من المدخلات
دالة تسجيل(الاسم, العمر)
إذا (طول(الاسم) < 2)
    ارمي "الاسم قصير جداً"
نهاية
إذا (العمر < 0 أو العمر > 150)
    ارمي "عمر غير صالح: " + العمر
نهاية
اطبع_سطر(f"تم تسجيل {الاسم} ✓")
نهاية

حاول
تسجيل("أ", 25)
امسك (خ)
اطبع_سطر("خطأ: " + خ)
نهاية`,exercise:`# تمرين: اكتب دالة "تحقق_كلمة_سر" ترمي أخطاء:
# - أقل من 8 أحرف: "كلمة السر قصيرة"
# - أطول من 50: "كلمة السر طويلة"
# - فارغة: "كلمة السر مطلوبة"
`,hint:'استخدم ارمي "رسالة الخطأ"',tips:["استخدم ارمي للتحقق من المدخلات الخاطئة","رسالة الخطأ يجب أن تكون واضحة ومحددة","من يستدعي الدالة يتعامل مع الخطأ بـ حاول/امسك"],concepts:["ارمي","throw","استثناءات مخصصة"]},{id:"l2_41",title:"أنماط معالجة الأخطاء",desc:"أنماط شائعة للتعامل مع الأخطاء",explanation:"ثلاثة أنماط ذهبية: 1) القيمة الافتراضية — إذا فشل، ارجع قيمة بديلة. 2) إعادة المحاولة — للعمليات التي قد تنجح إذا أعدت المحاولة (كالشبكة). 3) Guard Clause — تحقق في البداية وارمِ إن لزم.",code:`# النمط 1: القيمة الافتراضية
دالة اقرأ_إعداد(إعدادات, المفتاح, افتراضي)
حاول
    ارجع إعدادات[المفتاح]
امسك (خطأ)
    ارجع افتراضي
نهاية
نهاية

# النمط 2: إعادة المحاولة
دالة اتصل_بمحاولات(محاولات_قصوى)
متغير ع = 0
بينما (ع < محاولات_قصوى)
    حاول
        اطبع_سطر(f"محاولة {ع + 1}...")
        # محاكاة عملية تفشل أحياناً
        إذا (ع < 2)
            ارمي "فشل الاتصال"
        نهاية
        اطبع_سطر("نجح الاتصال! ✓")
        ارجع صحيح
    امسك (خطأ)
        اطبع_سطر(f"  فشل: {خطأ}")
        ع += 1
    نهاية
نهاية
اطبع_سطر("فشلت كل المحاولات ✗")
ارجع خطأ
نهاية

اتصل_بمحاولات(3)

# النمط 3: الارجاع المبكر (Guard Clause)
دالة سجل_طالب(الاسم, الدرجة)
إذا (الاسم == لاشيء أو الاسم == "")
    ارمي "الاسم مطلوب"
نهاية
إذا (الدرجة < 0 أو الدرجة > 100)
    ارمي "درجة غير صالحة"
نهاية

# الكود الرئيسي — بعد كل التحققات
اطبع_سطر(f"تم تسجيل {الاسم} بدرجة {الدرجة}")
نهاية`,exercise:`# تمرين: اكتب دالة "شراء_منتج" تطبق أنماط الأخطاء:
# - تحقق من صحة المدخلات (Guard)
# - قيمة افتراضية للكمية
# - إعادة محاولة إذا فشلت عملية الدفع
`,hint:"ادمج الأنماط الثلاثة في دالة واحدة",tips:["Guard Clause = تحقق أولاً، نفّذ لاحقاً","إعادة المحاولة للعمليات التي قد تفشل مؤقتاً (شبكة...)","القيمة الافتراضية لتجنب توقف البرنامج"],concepts:["أنماط الأخطاء","guard clause","retry"]},{id:"l2_42",title:"أخطاء مخصصة",desc:"أنشئ أنواع أخطاء خاصة بتطبيقك",explanation:'بدلاً من رسائل غامضة، أنشئ رسائل واضحة تبدأ بنوع الخطأ: "بريد_غير_صالح: يجب أن يحتوي @". هذا يسهل تصنيف الأخطاء والتعامل مع كل نوع بطريقة مختلفة. في المستوى الثالث سنتعلم الأصناف لبناء أخطاء مهيكلة أفضل.',code:`# خطأ مخصص كنص وصفي
دالة تحقق_بريد(البريد)
إذا (ليس يحتوي_على(البريد, "@"))
    ارمي "بريد_غير_صالح: يجب أن يحتوي @"
نهاية
إذا (ليس ينتهي_ب(البريد, ".com") و ليس ينتهي_ب(البريد, ".net"))
    ارمي "نطاق_غير_مدعوم: يجب .com أو .net"
نهاية
ارجع صحيح
نهاية

# التعامل مع أخطاء محددة
دالة سجل_مستخدم(الاسم, البريد, كلمة_السر)
حاول
    إذا (طول(الاسم) < 2)
        ارمي "اسم_قصير"
    نهاية
    تحقق_بريد(البريد)
    إذا (طول(كلمة_السر) < 8)
        ارمي "كلمة_سر_ضعيفة"
    نهاية
    
    اطبع_سطر(f"✓ تم تسجيل {الاسم} بنجاح!")
امسك (خطأ)
    إذا (يبدأ_ب(خطأ, "بريد"))
        اطبع_سطر(f"⚠️ مشكلة في البريد: {خطأ}")
    وإلا إذا (يبدأ_ب(خطأ, "اسم"))
        اطبع_سطر(f"⚠️ مشكلة في الاسم: {خطأ}")
    وإلا
        اطبع_سطر(f"❌ خطأ: {خطأ}")
    نهاية
نهاية
نهاية

سجل_مستخدم("أحمد", "ahmed@mail.com", "strongPass123")
سجل_مستخدم("أ", "bad", "123")`,exercise:`# تمرين: ابنِ نظام أخطاء لبنك:
# - "رصيد_غير_كافٍ"
# - "حساب_محظور"
# - "مبلغ_سالب"
# وتعامل مع كل خطأ بطريقة مختلفة
`,hint:"استخدم يبدأ_ب أو يحتوي_على للتمييز بين الأخطاء",tips:['ابدأ رسالة الخطأ بنوع واضح: "نوع_الخطأ: التفاصيل"',"في المستوى 3 سنتعلم الأصناف — أخطاء مهيكلة أفضل"],concepts:["أخطاء مخصصة","تصنيف الأخطاء"]},{id:"l2_43",title:"تمرين: برنامج متين",desc:"ابنِ تطبيقاً يتعامل مع كل الأخطاء المحتملة",explanation:"البرنامج المتين لا يتوقف عند الأخطاء — يلتقطها ويتعامل معها بذكاء. هذا المشروع يجمع كل ما تعلمته عن الأخطاء: تحقق من المدخلات، ارمِ عند المشاكل، امسك وعالج. البرمجة المتينة علامة المحترفين!",code:`# نظام حسابات بنكية — متين ضد الأخطاء
متغير حسابات = {
"أحمد": 5000,
"سارة": 8000,
"محمد": 3000
}

دالة إيداع(الاسم, المبلغ)
إذا (ليس الاسم في حسابات)
    ارمي "حساب غير موجود: " + الاسم
نهاية
إذا (المبلغ <= 0)
    ارمي "مبلغ غير صالح: " + المبلغ
نهاية
حسابات[الاسم] += المبلغ
اطبع_سطر(f"✓ تم إيداع {المبلغ} لـ{الاسم}. الرصيد: {حسابات[الاسم]}")
نهاية

دالة سحب_بنكي(الاسم, المبلغ)
إذا (ليس الاسم في حسابات)
    ارمي "حساب غير موجود"
نهاية
إذا (المبلغ <= 0)
    ارمي "مبلغ غير صالح"
نهاية
إذا (المبلغ > حسابات[الاسم])
    ارمي f"رصيد غير كافٍ. المتاح: {حسابات[الاسم]}"
نهاية
حسابات[الاسم] -= المبلغ
اطبع_سطر(f"✓ تم سحب {المبلغ}. الرصيد: {حسابات[الاسم]}")
نهاية

# اختبار العمليات
متغير عمليات = [
["إيداع", "أحمد", 1000],
["سحب", "سارة", 500],
["سحب", "محمد", 5000],
["إيداع", "زيد", 100]
]

لكل عملية في عمليات
حاول
    إذا (عملية[0] == "إيداع")
        إيداع(عملية[1], عملية[2])
    وإلا
        سحب_بنكي(عملية[1], عملية[2])
    نهاية
امسك (خطأ)
    اطبع_سطر(f"⚠️ فشلت العملية: {خطأ}")
نهاية
نهاية`,exercise:`# تمرين: أضف للنظام:
# 1. دالة تحويل(من, إلى, مبلغ)
# 2. سجل عمليات (مصفوفة تسجل كل عملية)
# 3. دالة كشف_حساب(اسم)
`,hint:"التحويل = سحب من حساب + إيداع في حساب آخر",tips:["البرنامج المتين لا يتوقف عند الأخطاء","سجّل كل عملية لتتبع المشاكل","تحقق من المدخلات قبل أي عملية"],concepts:["برنامج متين","تطبيق شامل","أخطاء"]}]},lc={id:"l2_s6",title:"الملفات والإدخال/الإخراج",icon:"📁",lessons:[{id:"l2_44",title:"قراءة ملف نصي",desc:"اقرأ محتوى ملف نصي بالكامل",explanation:'الملفات تحفظ البيانات بشكل دائم — حتى بعد إغلاق البرنامج. "اقرأ_ملف" تجلب المحتوى كنص واحد، ما "اقرأ_أسطر" تُرجع مصفوفة أسطر. دائماً استخدم حاول/امسك مع الملفات لأن الملف قد لا يكون موجوداً!',code:`# قراءة ملف كامل
حاول
متغير محتوى = اقرأ_ملف("بيانات.txt")
اطبع_سطر("محتوى الملف:")
اطبع_سطر(محتوى)
امسك (خطأ)
اطبع_سطر("لم يتم العثور على الملف!")
نهاية

# قراءة ملف كأسطر
حاول
متغير أسطر = اقرأ_أسطر("بيانات.txt")
اطبع_سطر(f"عدد الأسطر: {طول(أسطر)}")
لكل سطر في أسطر
    اطبع_سطر("  > " + سطر)
نهاية
امسك (خطأ)
اطبع_سطر("خطأ في القراءة: " + خطأ)
نهاية`,exercise:`# تمرين: اقرأ ملفاً واحسب:
# 1. عدد الأسطر
# 2. عدد الكلمات
# 3. عدد الحروف
`,hint:"اقرأ كأسطر ثم حلل كل سطر",tips:["دائماً استخدم حاول/امسك مع الملفات","اقرأ_ملف تُرجع النص كاملاً","اقرأ_أسطر تُرجع مصفوفة أسطر"],concepts:["اقرأ_ملف","اقرأ_أسطر","قراءة ملف"]},{id:"l2_45",title:"كتابة ملف نصي",desc:"اكتب محتوى جديد في ملف",explanation:'"اكتب_ملف" تُنشئ ملفاً جديداً أو تستبدل القديم بالكامل. انتبه: المحتوى القديم يضيع! لحفظ مصفوفة كأسطر، استخدم "صل" لدمجها بسطر جديد (\\n) بينها. الكتابة للملفات تحفظ عملك بشكل دائم.',code:`# كتابة ملف جديد (يحل محل المحتوى القديم)
اكتب_ملف("ناتج.txt", "مرحبا بالعالم!\\nسطر ثاني")
اطبع_سطر("✓ تم كتابة الملف")

# كتابة مصفوفة كأسطر
متغير بيانات = ["أحمد: 85", "سارة: 92", "محمد: 78"]
متغير محتوى = صل(بيانات, "\\n")
اكتب_ملف("درجات.txt", محتوى)
اطبع_سطر("✓ تم حفظ الدرجات")

# التحقق من الكتابة
حاول
متغير قراءة = اقرأ_ملف("درجات.txt")
اطبع_سطر("المحتوى المكتوب:")
اطبع_سطر(قراءة)
امسك (خطأ)
اطبع_سطر("خطأ: " + خطأ)
نهاية`,exercise:`# تمرين: اكتب برنامج يُنشئ ملف تقرير:
# - عنوان
# - تاريخ
# - قائمة نتائج
# - خلاصة
`,hint:"ابنِ النص كاملاً ثم اكتبه دفعة واحدة",tips:["اكتب_ملف تحل محل المحتوى القديم بالكامل!","للإلحاق (إضافة بدون حذف) استخدم ألحق_ملف"],concepts:["اكتب_ملف","كتابة","حفظ"]},{id:"l2_46",title:"الإلحاق بملف",desc:"أضف محتوى لملف موجود بدون حذف القديم",explanation:'"ألحق_ملف" تُضيف في نهاية الملف بدون حذف الموجود — مثالية للسجلات (logs) واليوميات. كل حدث جديد يُضاف في النهاية مع الحفاظ على التاريخ. لا تنسِ \\n للسطر الجديد!',code:`# إنشاء ملف سجل
اكتب_ملف("سجل.txt", "=== سجل الأحداث ===\\n")

# إلحاق أحداث
ألحق_ملف("سجل.txt", "حدث 1: تم تشغيل النظام\\n")
ألحق_ملف("سجل.txt", "حدث 2: تسجيل دخول المستخدم\\n")
ألحق_ملف("سجل.txt", "حدث 3: تنفيذ عملية\\n")

# قراءة السجل
متغير سجل = اقرأ_ملف("سجل.txt")
اطبع_سطر(سجل)

# دالة تسجيل متقدمة
دالة سجّل(الرسالة)
ألحق_ملف("سجل.txt", f"[INFO] {الرسالة}\\n")
اطبع_سطر(f"📝 {الرسالة}")
نهاية

سجّل("بدء المعالجة")
سجّل("اكتمال المعالجة")`,exercise:`# تمرين: اكتب نظام مذكرات يومية:
# - إضافة ملاحظة جديدة (مع التاريخ)
# - قراءة كل الملاحظات
# - عدّ الملاحظات
`,hint:"استخدم ألحق_ملف لإضافة ملاحظات وتاريخ_اليوم() للتاريخ",tips:["ألحق_ملف تُضيف في نهاية الملف بدون حذف القديم","مفيدة للسجلات (logs) والتاريخ"],concepts:["ألحق_ملف","إلحاق","سجلات"]},{id:"l2_47",title:"قراءة الأسطر ومعالجتها",desc:"اقرأ ملفاً سطراً سطراً وعالج كل سطر",explanation:'معالجة الملفات سطراً بسطر أساس قراءة CSV وملفات الإعدادات. "اقرأ_أسطر" + حلقة "لكل" + "قسم" = قوة في تحليل البيانات. لا تنسِ "قص_أطراف" لتنظيف كل سطر وتخطي الفارغة منها.',code:`# معالجة ملف CSV بسيط
# لنفترض أن الملف يحتوي:
# أحمد,85
# سارة,92
# محمد,78

دالة حلل_درجات(اسم_ملف)
حاول
    متغير أسطر = اقرأ_أسطر(اسم_ملف)
    متغير طلاب = []
    
    لكل سطر في أسطر
        متغير نظيف = قص_أطراف(سطر)
        إذا (طول(نظيف) == 0)
            استمر    # تخطَّ الأسطر الفارغة
        نهاية
        
        متغير أجزاء = قسم(نظيف, ",")
        أضف(طلاب, {
            "الاسم": أجزاء[0],
            "الدرجة": لرقم(أجزاء[1])
        })
    نهاية
    
    ارجع طلاب
امسك (خطأ)
    اطبع_سطر("خطأ: " + خطأ)
    ارجع []
نهاية
نهاية

# إنشاء ملف للاختبار
اكتب_ملف("طلاب.csv", "أحمد,85\\nسارة,92\\nمحمد,78")

# التحليل والعرض
متغير نتائج = حلل_درجات("طلاب.csv")
لكل ط في نتائج
اطبع_سطر(f"  {ط['الاسم']}: {ط['الدرجة']}")
نهاية`,exercise:`# تمرين: اكتب دالة تقرأ ملف إعدادات بصيغة:
# مفتاح=قيمة
# وترجع خريطة بالإعدادات
`,hint:'قسم كل سطر بالفاصل "=" واستخدم الجزء الأول كمفتاح',tips:["تحقق من الأسطر الفارغة والتعليقات (#)","التنظيف بـ قص_أطراف مهم جداً لقراءة الملفات"],concepts:["معالجة أسطر","CSV","ملفات إعدادات"]},{id:"l2_48",title:"التحقق من وجود ملف",desc:"تحقق من وجود ملف أو مجلد قبل العمل عليه",explanation:'قبل قراءة ملف، تحقق من وجوده بـ "ملف_موجود". هذا يجنبك الأخطاء ويتيح لك إنشاء ملف افتراضي إذا لم يكن موجوداً. نمط "اقرأ أو أنشئ" شائع جداً في التطبيقات الحقيقية.',code:`# التحقق من وجود ملف
متغير اسم_ملف = "بيانات.txt"

إذا (ملف_موجود(اسم_ملف))
اطبع_سطر(f"الملف '{اسم_ملف}' موجود ✓")
متغير محتوى = اقرأ_ملف(اسم_ملف)
اطبع_سطر(محتوى)
وإلا
اطبع_سطر(f"الملف '{اسم_ملف}' غير موجود ✗")
# إنشاء الملف
اكتب_ملف(اسم_ملف, "ملف جديد")
اطبع_سطر("تم إنشاء الملف ✓")
نهاية

# نمط: قراءة أو إنشاء
دالة اقرأ_أو_أنشئ(المسار, المحتوى_الافتراضي)
إذا (ملف_موجود(المسار))
    ارجع اقرأ_ملف(المسار)
وإلا
    اكتب_ملف(المسار, المحتوى_الافتراضي)
    ارجع المحتوى_الافتراضي
نهاية
نهاية

متغير إعدادات = اقرأ_أو_أنشئ("إعدادات.txt", "اللغة=العربية\\nالثيم=داكن")
اطبع_سطر(إعدادات)`,exercise:`# تمرين: اكتب دالة نسخ_ملف(المصدر, الهدف) تنسخ ملفاً
# تحقق من وجود المصدر أولاً
`,hint:"اقرأ المصدر واكتبه في الهدف",tips:["دائماً تحقق من وجود الملف قبل قراءته","أنشئ ملفات افتراضية إذا لم تكن موجودة"],concepts:["ملف_موجود","تحقق من الملفات"]},{id:"l2_49",title:"إدارة المجلدات",desc:"أنشئ واقرأ وتنقل بين المجلدات",explanation:'المجلدات تنظم الملفات. "أنشئ_مجلد" لإنشاء جديد، "محتويات_مجلد" لسرد الملفات، "المجلد_الحالي" لمعرفة أين أنت. تعامل مع الأخطاء دائماً — المجلد قد يكون موجوداً مسبقاً أو بدون صلاحيات.',code:`# إنشاء مجلد
حاول
أنشئ_مجلد("مشروعي")
اطبع_سطر("✓ تم إنشاء المجلد")
امسك (خطأ)
اطبع_سطر("المجلد موجود أو خطأ: " + خطأ)
نهاية

# قراءة محتويات مجلد
حاول
متغير ملفات = محتويات_مجلد(".")
اطبع_سطر("ملفات المجلد الحالي:")
لكل ملف في ملفات
    اطبع_سطر("  📄 " + ملف)
نهاية
امسك (خطأ)
اطبع_سطر("خطأ: " + خطأ)
نهاية

# الحصول على المسار الحالي
اطبع_سطر("المجلد الحالي: " + المجلد_الحالي())`,exercise:`# تمرين: اكتب أداة "منظم ملفات" بسيطة:
# - اسرد ملفات المجلد الحالي
# - أنشئ مجلداً فرعياً
# - انقل ملفاً لمجلد
`,hint:"استخدم محتويات_مجلد وأنشئ_مجلد",tips:["تعامل دائماً مع الأخطاء عند العمل مع نظام الملفات",'". " تعني المجلد الحالي'],concepts:["أنشئ_مجلد","محتويات_مجلد","المجلد_الحالي"]},{id:"l2_50",title:"تمرين: دفتر ملاحظات",desc:"مشروع نهائي — تطبيق دفتر ملاحظات مع حفظ وتحميل",explanation:"مبروك! هذا المشروع يجمع كل ما تعلمته في المستوى الثاني: الدوال، المصفوفات، النصوص، الخرائط، معالجة الأخطاء، والملفات. تطبيق حقيقي يحفظ بياناته بشكل دائم! المستوى الثالث: البرمجة الكائنية! 🎉",code:`# 📓 تطبيق دفتر ملاحظات
ثابت اسم_الملف = "ملاحظات.txt"

دالة أضف_ملاحظة(النص)
ألحق_ملف(اسم_الملف, f"• {النص}\\n")
اطبع_سطر(f"✓ تمت إضافة الملاحظة")
نهاية

دالة عرض_ملاحظات()
إذا (ليس ملف_موجود(اسم_الملف))
    اطبع_سطر("لا توجد ملاحظات بعد")
    ارجع
نهاية

متغير محتوى = اقرأ_ملف(اسم_الملف)
اطبع_سطر("╔═══ ملاحظاتك ═══╗")
اطبع_سطر(محتوى)
اطبع_سطر("╚═════════════════╝")
نهاية

دالة عدد_الملاحظات()
إذا (ليس ملف_موجود(اسم_الملف))
    ارجع 0
نهاية
متغير أسطر = اقرأ_أسطر(اسم_الملف)
متغير عداد = 0
لكل سطر في أسطر
    إذا (يبدأ_ب(قص_أطراف(سطر), "•"))
        عداد += 1
    نهاية
نهاية
ارجع عداد
نهاية

دالة مسح_الكل()
اكتب_ملف(اسم_الملف, "")
اطبع_سطر("✓ تم مسح كل الملاحظات")
نهاية

# اختبار التطبيق
مسح_الكل()
أضف_ملاحظة("تعلّم لغة ص")
أضف_ملاحظة("بناء مشروع تطبيقي")
أضف_ملاحظة("نشر التطبيق")
عرض_ملاحظات()
اطبع_سطر(f"عدد الملاحظات: {عدد_الملاحظات()}")`,exercise:`# تمرين: وسّع التطبيق:
# 1. بحث في الملاحظات
# 2. حذف ملاحظة محددة (بالرقم)
# 3. تصدير لملف CSV
`,hint:"للحذف: اقرأ كل الأسطر، أزل المطلوب، أعد الكتابة",tips:["مبروك! أتممت المستوى الثاني 🎉","أصبح لديك أساس متين في الدوال والمصفوفات والأخطاء والملفات","المستوى الثالث: البرمجة الكائنية!"],concepts:["مشروع شامل","ملفات","CRUD"]}]},cc={id:2,title:"المبتدئ",titleEn:"Beginner",description:"أتقن الأدوات الأساسية: الدوال والمصفوفات والأخطاء",icon:"🌿",color:"#3b82f6",sections:[nc,sc,oc,ic,ac,lc]},rc={id:"l3_classes_intro",title:"مقدمة في الأصناف",icon:"🏗️",lessons:[{id:"l3_01",title:"ما هو الصنف؟",desc:"مفهوم الأصناف والبرمجة الكائنية — تنظيم الكود حول الكائنات.",explanation:'تخيّل الصنف كقالب أو مخطط بناء: المخطط ليس منزلاً حقيقياً، لكنه يصف كيف يبدو المنزل. عندما تُنشئ كائناً من صنف، فأنت تبني "منزلاً" من ذلك المخطط. البرمجة الكائنية تنظم الكود حول هذه الكائنات بدلاً من مجرد قوائم أوامر، مما يجعل الكود أشبه بالعالم الحقيقي ويسهّل صيانته وتوسيعه.',code:`# الصنف هو قالب لإنشاء كائنات
# تخيل الصنف كمخطط بناء منزل
# كل منزل (كائن) يُبنى من نفس المخطط

صنف سيارة
    باني()
        هذا.لون = "أحمر"
        هذا.سرعة = 0
    نهاية

    دالة وصف()
        اطبع("سيارة " + هذا.لون)
    نهاية
نهاية

# إنشاء كائن من الصنف
متغير سيارتي = جديد سيارة()
سيارتي.وصف()
اطبع("السرعة: " + سيارتي.سرعة)`,exercise:'أنشئ صنف "كتاب" يحتوي على عنوان ومؤلف ودالة وصف تطبع بيانات الكتاب.',hint:"استخدم باني() لتعيين هذا.عنوان وهذا.مؤلف، ثم أنشئ دالة وصف() تستخدم اطبع().",tips:['الصنف يُعرَّف بكلمة "صنف" متبوعة بالاسم','كلمة "جديد" تُنشئ كائناً من الصنف','"هذا" تُشير إلى الكائن الحالي'],concepts:["صنف","كائن","جديد","هذا"]},{id:"l3_02",title:"الباني — تهيئة الكائن",desc:"كيف يعمل الباني (Constructor) لتهيئة خصائص الكائن عند إنشائه.",explanation:"الباني كأنه طقس الميلاد للكائن — يُستدعى تلقائياً لحظة الإنشاء ويجهّز كل شيء. مثلما يُسجَّل اسم المولود وتاريخ ولادته فور ولادته، الباني يعيّن القيم الأولية لخصائص الكائن. بدون الباني، ستحتاج لتعيين كل خاصية يدوياً بعد الإنشاء — أما معه فكل شيء جاهز من البداية!",code:`# الباني يُستدعى تلقائياً عند إنشاء كائن جديد
صنف شخص
    باني(الاسم، العمر)
        هذا.الاسم = الاسم
        هذا.العمر = العمر
        هذا.نشط = صحيح
    نهاية

    دالة تعريف()
        ارجع "أنا " + هذا.الاسم + " عمري " + هذا.العمر
    نهاية
نهاية

متغير أحمد = جديد شخص("أحمد"، 25)
متغير سارة = جديد شخص("سارة"، 30)

اطبع(أحمد.تعريف())
اطبع(سارة.تعريف())
اطبع(أحمد.نشط)`,exercise:'أنشئ صنف "طالب" بباني يأخذ الاسم والدرجة، ودالة "ناجح" ترجع صحيح إذا الدرجة >= 50.',hint:"استخدم باني(الاسم، الدرجة) وفي دالة ناجح() ارجع هذا.الدرجة >= 50.",tips:['الباني يُعرَّف بكلمة "باني" داخل الصنف',"المعاملات تُمرر عند استدعاء جديد","يمكن تعيين قيم إضافية داخل الباني"],concepts:["باني","معاملات","تهيئة"]},{id:"l3_03",title:"الخصائص والدوال",desc:"التفريق بين خصائص الكائن (البيانات) ودواله (السلوك).",explanation:'الخصائص هي "ما يملكه" الكائن (مثل اسم الشخص، رصيده)، بينما الدوال هي "ما يفعله" (مثل إيداع مبلغ، سحب مال). فكّر في هاتفك: خصائصه هي اللون ونسبة البطارية، ودواله هي الاتصال وإرسال رسالة. هذا الفصل بين البيانات والسلوك يجعل الكود منظماً ومفهوماً.',code:`صنف حساب_بنكي
    باني(المالك، الرصيد)
        هذا.المالك = المالك
        هذا.الرصيد = الرصيد
        هذا.العمليات = []
    نهاية

    دالة إيداع(مبلغ)
        هذا.الرصيد += مبلغ
        هذا.العمليات.أضف("إيداع: " + مبلغ)
        اطبع("تم إيداع " + مبلغ)
    نهاية

    دالة سحب(مبلغ)
        إذا (مبلغ > هذا.الرصيد)
            اطبع("رصيد غير كافٍ!")
            ارجع خطأ
        نهاية
        هذا.الرصيد -= مبلغ
        هذا.العمليات.أضف("سحب: " + مبلغ)
        اطبع("تم سحب " + مبلغ)
        ارجع صحيح
    نهاية

    دالة عرض_الرصيد()
        اطبع(هذا.المالك + " — الرصيد: " + هذا.الرصيد)
    نهاية
نهاية

متغير حسابي = جديد حساب_بنكي("أحمد"، 1000)
حسابي.إيداع(500)
حسابي.سحب(200)
حسابي.عرض_الرصيد()`,exercise:'أنشئ صنف "عداد" يحتوي على قيمة، ودوال: زيادة() ونقصان() وقراءة() وصفر().',hint:"عيّن هذا.قيمة = 0 في الباني، دالة زيادة تضيف 1، نقصان تطرح 1، صفر تعيد القيمة لصفر.",tips:["الخصائص = البيانات (هذا.الاسم، هذا.الرصيد)","الدوال = السلوك (إيداع، سحب، عرض)","دوال تُرجع قيمة أو تطبع أو تعدّل الحالة"],concepts:["خصائص","دوال","حالة الكائن"]},{id:"l3_04",title:'كلمة "هذا" بالتفصيل',desc:'فهم عميق لكلمة "هذا" (this) وكيف تُشير للكائن الحالي.',explanation:'كلمة "هذا" كأنها إصبع يشير إلى "أنا" — الكائن الذي يتحدث الآن. عندما تقول "هذا.الاسم" داخل دالة، أنت تقول "اسمي أنا". تخيّل موظفين في شركة، كلاهما يقول "راتبي" — كل واحد يقصد راتبه هو. "هذا" تحل هذا الالتباس وتربط الدالة بالكائن الذي استدعاها.',code:`صنف لاعب
    باني(الاسم، المستوى)
        هذا.الاسم = الاسم
        هذا.المستوى = المستوى
        هذا.النقاط = 0
    نهاية

    دالة اكسب_نقاط(نقاط)
        هذا.النقاط += نقاط
        # هذا يُشير لنفس الكائن
        إذا (هذا.النقاط >= 100)
            هذا.ترقية()
        نهاية
    نهاية

    دالة ترقية()
        هذا.المستوى += 1
        هذا.النقاط = 0
        اطبع(هذا.الاسم + " ترقى للمستوى " + هذا.المستوى)
    نهاية

    دالة حالة()
        اطبع(هذا.الاسم + " | مستوى: " + هذا.المستوى + " | نقاط: " + هذا.النقاط)
    نهاية
نهاية

متغير لاعب1 = جديد لاعب("خالد"، 1)
لاعب1.اكسب_نقاط(50)
لاعب1.حالة()
لاعب1.اكسب_نقاط(60)
لاعب1.حالة()`,exercise:'أنشئ صنف "مؤقت" يحتوي على ثوانٍ، ودالة بدء() تزيد الثواني، ودالة عرض() تطبع الوقت.',hint:"استخدم هذا.ثواني في كل الدوال للوصول لنفس البيانات.",tips:['"هذا" تربط الدالة بالكائن الذي استدعاها','بدون "هذا" لن تصل للخصائص',"دالة يمكنها استدعاء دوال أخرى عبر هذا.دالة()"],concepts:["هذا","مرجع ذاتي","استدعاء داخلي"]},{id:"l3_05",title:"كائنات متعددة",desc:"إنشاء عدة كائنات من نفس الصنف — كل كائن مستقل.",explanation:"كل كائن كأنه نسخة مستقلة تماماً — مثل طابعة تطبع استمارات فارغة، كل استمارة تُملأ ببيانات مختلفة. إذا عدّلت بيانات كائن، الكائنات الأخرى لا تتأثر! هذه الاستقلالية قوة كبيرة: يمكنك إنشاء آلاف المستخدمين، كل واحد برصيده واسمه الخاص، دون أن يتداخلوا.",code:`صنف حيوان_أليف
    باني(الاسم، النوع، العمر)
        هذا.الاسم = الاسم
        هذا.النوع = النوع
        هذا.العمر = العمر
        هذا.جائع = صحيح
    نهاية

    دالة أطعم()
        هذا.جائع = خطأ
        اطبع(هذا.الاسم + " أكل 🍽️")
    نهاية

    دالة بطاقة()
        متغير حالة = هذا.جائع ? "جائع 😿" : "شبعان 😺"
        اطبع(هذا.الاسم + " (" + هذا.النوع + ") — " + حالة)
    نهاية
نهاية

# كل كائن مستقل تماماً
متغير قطتي = جديد حيوان_أليف("ميمي"، "قطة"، 3)
متغير كلبي = جديد حيوان_أليف("ريكس"، "كلب"، 5)

قطتي.بطاقة()
كلبي.بطاقة()

# أطعم القطة فقط
قطتي.أطعم()

قطتي.بطاقة()
كلبي.بطاقة()  # لا يزال جائعاً!`,exercise:'أنشئ مصفوفة من 3 كائنات "طالب" مختلفة، ثم استخدم حلقة لطباعة اسم ودرجة كل طالب.',hint:"أنشئ المصفوفة: متغير طلاب = [جديد طالب(...), جديد طالب(...), جديد طالب(...)], ثم لكل ط في طلاب.",tips:["كل كائن يحتفظ ببياناته مستقلاً","تعديل كائن لا يؤثر على الآخر","يمكن تخزين الكائنات في مصفوفات"],concepts:["كائنات مستقلة","مصفوفة كائنات"]},{id:"l3_06",title:"الكائنات كمعاملات",desc:"تمرير الكائنات كمعاملات للدوال والتفاعل بينها.",explanation:'الكائنات يمكن أن تتفاعل مع بعضها البعض — كسلة تسوق تستقبل منتجات! عندما تمرر كائناً كمعامل، أنت تمرر "المرجع" لا "نسخة"، أي أن أي تعديل يحدث على الكائن الأصلي. هذا النمط يُسمى "التركيب" — بناء أنظمة معقدة من كائنات بسيطة تتعاون معاً.',code:`صنف منتج
    باني(الاسم، السعر)
        هذا.الاسم = الاسم
        هذا.السعر = السعر
    نهاية
نهاية

صنف سلة_تسوق
    باني()
        هذا.المنتجات = []
    نهاية

    دالة أضف(منتج)
        هذا.المنتجات.أضف(منتج)
        اطبع("تمت إضافة: " + منتج.الاسم)
    نهاية

    دالة المجموع()
        متغير مجموع = 0
        لكل م في هذا.المنتجات
            مجموع += م.السعر
        نهاية
        ارجع مجموع
    نهاية

    دالة عرض()
        اطبع("=== السلة ===")
        لكل م في هذا.المنتجات
            اطبع(م.الاسم + ": " + م.السعر + " ريال")
        نهاية
        اطبع("المجموع: " + هذا.المجموع() + " ريال")
    نهاية
نهاية

متغير تفاح = جديد منتج("تفاح"، 15)
متغير حليب = جديد منتج("حليب"، 8)
متغير خبز = جديد منتج("خبز"، 5)

متغير سلة = جديد سلة_تسوق()
سلة.أضف(تفاح)
سلة.أضف(حليب)
سلة.أضف(خبز)
سلة.عرض()`,exercise:'أنشئ صنف "فريق" يحتوي على مصفوفة لاعبين، ودالة أضف_لاعب(لاعب) ودالة أفضل_لاعب() ترجع اللاعب الأعلى نقاطاً.',hint:"في دالة أفضل_لاعب() استخدم حلقة لمقارنة النقاط بين اللاعبين.",tips:["الكائنات تُمرر بالمرجع — تعديلها يؤثر على الأصل","يمكن تخزين كائنات داخل كائنات أخرى",'هذا النمط يسمى "التركيب" (Composition)'],concepts:["تمرير كائنات","تركيب","تفاعل بين كائنات"]},{id:"l3_07",title:"أنماط التصميم البسيطة",desc:"أنماط شائعة في تصميم الأصناف: التغليف والتركيب.",explanation:"التغليف (Encapsulation) كأنك تخفي محرك السيارة — السائق يستخدم المقود والدواسات فقط دون معرفة تفاصيل المحرك. التركيب (Composition) كأنك تبني سيارة من أجزاء: محرك + عجلات + هيكل. هذان النمطان أساسيان: أخفِ التعقيد ووفّر واجهة بسيطة، واجمع كائنات صغيرة لبناء أنظمة كبيرة.",code:`# نمط التغليف: إخفاء التفاصيل الداخلية
صنف كلمة_سر
    باني(نص)
        # نخزن كلمة السر داخلياً
        هذا.القيمة = نص
        هذا.المحاولات = 0
    نهاية

    دالة تحقق(محاولة)
        هذا.المحاولات += 1
        إذا (هذا.المحاولات > 3)
            اطبع("تم حظر الحساب!")
            ارجع خطأ
        نهاية
        ارجع محاولة == هذا.القيمة
    نهاية

    دالة عدد_المحاولات()
        ارجع هذا.المحاولات
    نهاية
نهاية

# نمط التركيب: بناء أنظمة من أجزاء
صنف محرك
    باني(القوة)
        هذا.القوة = القوة
        هذا.يعمل = خطأ
    نهاية
    دالة تشغيل()
        هذا.يعمل = صحيح
    نهاية
    دالة إيقاف()
        هذا.يعمل = خطأ
    نهاية
نهاية

صنف سيارة
    باني(الاسم، قوة_المحرك)
        هذا.الاسم = الاسم
        هذا.المحرك = جديد محرك(قوة_المحرك)
    نهاية
    دالة ابدأ()
        هذا.المحرك.تشغيل()
        اطبع(هذا.الاسم + " بدأت — محرك " + هذا.المحرك.القوة + " حصان")
    نهاية
نهاية

متغير سيارتي = جديد سيارة("تويوتا"، 150)
سيارتي.ابدأ()`,exercise:"أنشئ نظام مكتبة بسيط: صنف كتاب (عنوان، مؤلف) وصنف مكتبة (مصفوفة كتب) مع دوال إضافة وبحث وعرض.",hint:"صنف مكتبة يحتوي هذا.كتب = [] ودالة بحث(عنوان) تبحث في المصفوفة.",tips:["التغليف: أخفِ التفاصيل ووفر واجهة بسيطة","التركيب: اجمع كائنات صغيرة لبناء كائنات أكبر","كل صنف يجب أن يكون مسؤولاً عن شيء واحد"],concepts:["تغليف","تركيب","مسؤولية واحدة"]},{id:"l3_08",title:"مشروع: نظام إدارة المهام",desc:"تطبيق شامل يجمع كل مفاهيم الأصناف الأساسية.",explanation:'هذا المشروع يجمع كل ما تعلمته: صنف "مهمة" يمثل مهمة واحدة، وصنف "مدير_المهام" يدير مجموعة مهام. لاحظ كيف كل صنف له مسؤولية واضحة — المهمة تعرف كيف تصف نفسها، والمدير يعرف كيف يضيف ويكمل المهام. هذا التقسيم يجعل الكود سهل التوسيع والصيانة.',code:`# مشروع: نظام إدارة المهام
صنف مهمة
    باني(العنوان، الأولوية)
        هذا.العنوان = العنوان
        هذا.الأولوية = الأولوية  # 1=عاجل، 2=مهم، 3=عادي
        هذا.مكتملة = خطأ
    نهاية

    دالة أكمل()
        هذا.مكتملة = صحيح
    نهاية

    دالة وصف()
        متغير حالة = هذا.مكتملة ? "✅" : "⬜"
        متغير أولوية_نص = ""
        إذا (هذا.الأولوية == 1)
            أولوية_نص = "🔴 عاجل"
        وإلا
            إذا (هذا.الأولوية == 2)
                أولوية_نص = "🟡 مهم"
            وإلا
                أولوية_نص = "🟢 عادي"
            نهاية
        نهاية
        ارجع حالة + " [" + أولوية_نص + "] " + هذا.العنوان
    نهاية
نهاية

صنف مدير_المهام
    باني()
        هذا.المهام = []
    نهاية

    دالة أضف(عنوان، أولوية)
        هذا.المهام.أضف(جديد مهمة(عنوان، أولوية))
        اطبع("✅ تمت إضافة: " + عنوان)
    نهاية

    دالة أكمل_مهمة(رقم)
        إذا (رقم >= 0 و رقم < طول(هذا.المهام))
            هذا.المهام[رقم].أكمل()
        نهاية
    نهاية

    دالة عرض_الكل()
        اطبع("\\n📋 قائمة المهام:")
        متغير ع = 0
        لكل م في هذا.المهام
            اطبع(ع + ". " + م.وصف())
            ع += 1
        نهاية
    نهاية
نهاية

# التجربة
متغير مدير = جديد مدير_المهام()
مدير.أضف("مراجعة الكود"، 1)
مدير.أضف("كتابة التوثيق"، 2)
مدير.أضف("ترتيب الملفات"، 3)

مدير.عرض_الكل()
مدير.أكمل_مهمة(0)
مدير.عرض_الكل()`,exercise:'أضف دالة "عاجلة_فقط" لمدير المهام تعرض المهام ذات الأولوية 1 فقط، ودالة "الإحصائيات" تطبع عدد المهام المكتملة وغير المكتملة.',hint:"في دالة عاجلة_فقط() استخدم حلقة مع شرط م.الأولوية == 1. في الإحصائيات عُدّ المكتملة بشرط م.مكتملة == صحيح.",tips:["هذا المشروع يجمع: الأصناف، الباني، هذا، المصفوفات، الحلقات","كائن يمكنه إنشاء كائنات أخرى (مدير ينشئ مهام)","كل صنف له مسؤولية واضحة"],concepts:["مشروع متكامل","تكامل المفاهيم","تصميم"]}]},dc={id:"l3_adv_classes",title:"الأصناف المتقدمة",icon:"⚙️",lessons:[{id:"l3_09",title:"محددات الوصول — عام وخاص",desc:"التحكم في من يستطيع الوصول لدوال الصنف.",explanation:'تخيّل منزلاً: الباب الأمامي "عام" يدخله الضيوف، والغرف الخاصة "خاصة" للعائلة فقط. في البرمجة، "عام" يعني أي كود يمكنه استدعاء الدالة، و"خاص" يعني فقط الصنف نفسه. هذا يحمي التفاصيل الداخلية ويمنع الاستخدام الخاطئ — مبدأ أساسي لبناء كود آمن وموثوق.',code:`# محددات الوصول تتحكم في رؤية الدوال
صنف حساب
    باني(المالك، الرصيد)
        هذا.المالك = المالك
        هذا.الرصيد = الرصيد
    نهاية

    # دالة عامة — يمكن استدعاؤها من الخارج
    عام دالة احصل_رصيد()
        ارجع هذا.الرصيد
    نهاية

    عام دالة إيداع(مبلغ)
        إذا (مبلغ > 0)
            هذا.الرصيد += مبلغ
            هذا.سجل("إيداع"، مبلغ)
        نهاية
    نهاية

    # دالة خاصة — فقط من داخل الصنف
    خاص دالة سجل(نوع، مبلغ)
        اطبع("[سجل] " + نوع + ": " + مبلغ)
    نهاية
نهاية

متغير ح = جديد حساب("سارة"، 1000)
ح.إيداع(500)
اطبع("الرصيد: " + ح.احصل_رصيد())

# ح.سجل("اختراق"، 9999)  ← سيفشل! دالة خاصة`,exercise:'أنشئ صنف "خزنة" بدالة خاصة للتحقق من كلمة السر، ودالة عامة "افتح" تستقبل كلمة السر وتستخدم الدالة الخاصة.',hint:"خاص دالة تحقق(كلمة) تقارن بـ هذا.كلمة_السر، عام دالة افتح(كلمة) تستدعي هذا.تحقق(كلمة).",tips:['"عام" = مرئي للجميع (الافتراضي)','"خاص" = مرئي فقط داخل الصنف','"محمي" = مرئي للصنف والأصناف الوارثة'],concepts:["عام","خاص","محمي","تغليف"]},{id:"l3_10",title:"الدوال الساكنة",desc:'دوال تخص الصنف نفسه وليس الكائنات — لا تحتاج "جديد".',explanation:'الدالة الساكنة كآلة حاسبة موجودة في مكتبة — لا تحتاج لاستعارتها (جديد)، فقط استخدمها مباشرة! الدوال الساكنة لا تحتاج "هذا" لأنها لا تنتمي لكائن معين. مثالية لدوال المنفعة كحساب الجذر التربيعي أو تحويل درجة حرارة — عمليات لا تحتاج حالة خاصة.',code:`# الدالة الساكنة تنتمي للصنف وليس للكائن
صنف رياضيات
    ساكن دالة جمع(أ، ب)
        ارجع أ + ب
    نهاية

    ساكن دالة أكبر(أ، ب)
        إذا (أ > ب)
            ارجع أ
        نهاية
        ارجع ب
    نهاية

    ساكن دالة مطلق(ن)
        إذا (ن < 0)
            ارجع -ن
        نهاية
        ارجع ن
    نهاية

    ساكن دالة قوة(أساس، أس)
        متغير نتيجة = 1
        لكل ع في نطاق(0، أس)
            نتيجة *= أساس
        نهاية
        ارجع نتيجة
    نهاية
نهاية

# استدعاء بدون إنشاء كائن!
اطبع(رياضيات.جمع(10، 20))
اطبع(رياضيات.أكبر(5، 8))
اطبع(رياضيات.مطلق(-42))
اطبع(رياضيات.قوة(2، 10))`,exercise:'أنشئ صنف "محول" بدوال ساكنة: سلسيوس_لفهرنهايت(درجة)، كيلو_لمايل(كيلو)، ريال_لدولار(مبلغ).',hint:"فهرنهايت = سلسيوس * 9/5 + 32، مايل = كيلو * 0.621، دولار = ريال / 3.75.",tips:['الدوال الساكنة لا تستخدم "هذا"',"تُستدعى عبر اسم الصنف مباشرة","مفيدة لدوال المنفعة (Utility Functions)"],concepts:["ساكن","دوال المنفعة","بدون كائن"]},{id:"l3_11",title:"تحميل العوامل الزائد — الجزء 1",desc:"تعريف سلوك العوامل (+، -، *) لأصنافك الخاصة.",explanation:'عادةً نجمع أرقاماً بـ +، لكن ماذا لو أردت جمع متجهين أو كسرين؟ تحميل العوامل يتيح لك تعليم + كيف يعمل مع أصنافك! بدلاً من استدعاء دالة "اجمع_متجهات(أ، ب)"، بإمكانك ببساطة كتابة "أ + ب". هذا يجعل الكود أنظف وأشبه بالرياضيات الطبيعية.',code:`# تحميل العوامل يتيح استخدام + و- مع كائناتك
صنف متجه
    باني(س، ص)
        هذا.س = س
        هذا.ص = ص
    نهاية

    # تعريف عامل الجمع +
    عامل +(آخر)
        ارجع جديد متجه(هذا.س + آخر.س، هذا.ص + آخر.ص)
    نهاية

    # تعريف عامل الطرح -
    عامل -(آخر)
        ارجع جديد متجه(هذا.س - آخر.س، هذا.ص - آخر.ص)
    نهاية

    دالة طباعة()
        اطبع("(" + هذا.س + "، " + هذا.ص + ")")
    نهاية
نهاية

متغير أ = جديد متجه(3، 4)
متغير ب = جديد متجه(1، 2)

# الآن يمكنك استخدام + و- مع المتجهات!
متغير ج = أ + ب
ج.طباعة()   # (4، 6)

متغير د = أ - ب
د.طباعة()   # (2، 2)`,exercise:'أنشئ صنف "كسر" (بسط ومقام) وعرّف عامل الجمع + لجمع كسرين.',hint:"إذا كان أ/ب + ج/د = (أ*د + ج*ب)/(ب*د).",tips:['عامل + يُعرف بـ "عامل +(معامل)"',"يجب أن يُرجع كائناً جديداً","لا تعدّل الكائن الأصلي!"],concepts:["عامل","تحميل زائد","جمع كائنات"]},{id:"l3_12",title:"تحميل العوامل الزائد — الجزء 2",desc:"المزيد من العوامل: الضرب، المقارنة، والنفي.",explanation:'بعد + و-، نتعلم الضرب ("نقود * 3 = ثلاثة أضعاف") والمقارنة ("هل نقود1 == نقود2؟"). المقارنة مهمة خاصة — بدون تحميل ==، ستقارن المراجع لا القيم! المفتاح: كل عامل يأخذ معاملاً واحداً (الطرف الآخر)، وعوامل المقارنة تُرجع منطقي (صحيح/خطأ).',code:`صنف نقود
    باني(المبلغ، العملة)
        هذا.المبلغ = المبلغ
        هذا.العملة = العملة
    نهاية

    # ضرب: مضاعفة المبلغ
    عامل *(عامل_ضرب)
        ارجع جديد نقود(هذا.المبلغ * عامل_ضرب، هذا.العملة)
    نهاية

    # مقارنة: هل المبلغان متساويان؟
    عامل ==(آخر)
        ارجع هذا.المبلغ == آخر.المبلغ و هذا.العملة == آخر.العملة
    نهاية

    # أكبر من
    عامل >(آخر)
        ارجع هذا.المبلغ > آخر.المبلغ
    نهاية

    دالة طباعة()
        اطبع(هذا.المبلغ + " " + هذا.العملة)
    نهاية
نهاية

متغير سعر = جديد نقود(100، "ريال")
متغير مضاعف = سعر * 3
مضاعف.طباعة()

متغير سعر2 = جديد نقود(100، "ريال")
اطبع(سعر == سعر2)   # صحيح
اطبع(مضاعف > سعر)   # صحيح`,exercise:'أنشئ صنف "مصفوفة_رياضية" (2×2) وعرّف عامل + لجمع مصفوفتين وعامل == للمقارنة.',hint:"خزّن القيم في 4 خصائص (أ11، أ12، أ21، أ22) وفي عامل + اجمع كل عنصر مع نظيره.",tips:["يمكن تحميل: +، -، *، /، ==، !=، <، >، <=، >=","العامل == يجب أن يُرجع منطقي (صحيح/خطأ)","كل عامل يأخذ معاملاً واحداً (الطرف الآخر)"],concepts:["عوامل المقارنة","عامل الضرب","تصميم API"]},{id:"l3_13",title:"الأصناف مع المصفوفات",desc:"أنماط شائعة لاستخدام المصفوفات داخل الأصناف.",explanation:"الأصناف + المصفوفات = هياكل بيانات قوية! قائمة الانتظار (Queue) كطابور البنك: أول من يدخل أول من يُخدَم. المكدس (Stack) ككومة صحون: آخر ما تضيفه أول ما تأخذه. تعلّم هذه الأنماط يفتح لك الباب لبناء خوارزميات كتراجع المتصفح (undo) وإدارة الطلبات.",code:`صنف قائمة_الانتظار
    باني()
        هذا.العناصر = []
    نهاية

    دالة أدخل(عنصر)
        هذا.العناصر.أضف(عنصر)
    نهاية

    دالة أخرج()
        إذا (هذا.فارغة())
            اطبع("القائمة فارغة!")
            ارجع لاشيء
        نهاية
        # أخذ أول عنصر
        متغير أول = هذا.العناصر[0]
        هذا.العناصر = هذا.العناصر.شريحة(1)
        ارجع أول
    نهاية

    دالة فارغة()
        ارجع طول(هذا.العناصر) == 0
    نهاية

    دالة الحجم()
        ارجع طول(هذا.العناصر)
    نهاية
نهاية

# مكدس (Stack) — آخر ما يدخل أول ما يخرج
صنف مكدس
    باني()
        هذا.العناصر = []
    نهاية

    دالة ادفع(عنصر)
        هذا.العناصر.أضف(عنصر)
    نهاية

    دالة اسحب()
        إذا (طول(هذا.العناصر) == 0)
            ارجع لاشيء
        نهاية
        ارجع هذا.العناصر.شريحة(-1)[0]
    نهاية
نهاية

متغير ق = جديد قائمة_الانتظار()
ق.أدخل("أحمد")
ق.أدخل("سارة")
ق.أدخل("خالد")

اطبع(ق.أخرج())  # أحمد (أول من دخل)
اطبع(ق.أخرج())  # سارة`,exercise:'أنشئ صنف "سجل_درجات" يخزن درجات الطلاب ويوفر: أعلى_درجة()، أدنى_درجة()، المتوسط().',hint:"استخدم مصفوفة لتخزين الدرجات وحلقات لحساب الأعلى والأدنى والمتوسط.",tips:["هياكل البيانات الشائعة يمكن بناؤها بالأصناف","قائمة الانتظار: FIFO — أول من يدخل أول من يخرج","المكدس: LIFO — آخر من يدخل أول من يخرج"],concepts:["هياكل البيانات","قائمة انتظار","مكدس"]},{id:"l3_14",title:"الأصناف مع الخرائط",desc:"استخدام الخرائط (القواميس) داخل الأصناف لتنظيم البيانات.",explanation:"الخريطة كدفتر هاتف: الاسم (المفتاح) → الرقم (القيمة). داخل صنف، الخريطة مثالية للبيانات المفهرسة: قاموس كلمات، جهات اتصال، إعدادات. البحث بالمفتاح سريع جداً (فوري) مقارنة بالبحث في مصفوفة (تتطلب مرور على كل عنصر).",code:`صنف دفتر_هاتف
    باني()
        هذا.جهات_الاتصال = {}
    نهاية

    دالة أضف(الاسم، الرقم)
        هذا.جهات_الاتصال[الاسم] = الرقم
        اطبع("تمت إضافة: " + الاسم)
    نهاية

    دالة ابحث(الاسم)
        إذا (هذا.جهات_الاتصال.يحتوي(الاسم))
            ارجع هذا.جهات_الاتصال[الاسم]
        نهاية
        ارجع "غير موجود"
    نهاية

    دالة احذف(الاسم)
        إذا (هذا.جهات_الاتصال.يحتوي(الاسم))
            هذا.جهات_الاتصال.احذف(الاسم)
            اطبع("تم حذف: " + الاسم)
        نهاية
    نهاية

    دالة عرض_الكل()
        اطبع("📞 دفتر الهاتف:")
        لكل اسم في هذا.جهات_الاتصال.مفاتيح()
            اطبع("  " + اسم + ": " + هذا.جهات_الاتصال[اسم])
        نهاية
    نهاية
نهاية

متغير دفتر = جديد دفتر_هاتف()
دفتر.أضف("أحمد"، "0551234567")
دفتر.أضف("سارة"، "0557654321")
دفتر.أضف("خالد"، "0559876543")
دفتر.عرض_الكل()

اطبع("رقم أحمد: " + دفتر.ابحث("أحمد"))`,exercise:'أنشئ صنف "قاموس" يخزن كلمات ومعانيها، مع دوال: أضف_كلمة()، ابحث()، كلمات_تبدأ_بـ(حرف).',hint:"استخدم خريطة للتخزين، ولكلمات_تبدأ_بـ() استخدم حلقة على المفاتيح مع يبدأ_بـ().",tips:["الخرائط مثالية للبيانات المُفهرسة بمفتاح","يحتوي() للتحقق من وجود مفتاح","مفاتيح() تُرجع مصفوفة بجميع المفاتيح"],concepts:["خريطة داخل صنف","فهرسة","بحث"]},{id:"l3_15",title:"أنماط البناء",desc:"أنماط شائعة في بناء الأصناف: باني مرن ودوال متسلسلة.",explanation:'أحياناً لا نريد تحديد كل شيء في الباني — نريد قيماً افتراضية ذكية. مثل طلب قهوة: "قهوة" افتراضياً وسط، بسكر عادي. يمكنك تخصيص أي جزء (حجم كبير، بدون سكر) والباقي يبقى افتراضياً. هذا النمط (Builder) يجعل الأصناف سهلة الاستخدام ومرنة.',code:`# صنف مع باني مرن
صنف إعدادات
    باني()
        هذا.اللون = "أبيض"
        هذا.الحجم = 14
        هذا.الخط = "Cairo"
        هذا.داكن = خطأ
    نهاية

    دالة عيّن_لون(لون)
        هذا.اللون = لون
    نهاية

    دالة عيّن_حجم(حجم)
        هذا.الحجم = حجم
    نهاية

    دالة وضع_داكن()
        هذا.داكن = صحيح
        هذا.اللون = "أسود"
    نهاية

    دالة عرض()
        اطبع("اللون: " + هذا.اللون)
        اطبع("الحجم: " + هذا.الحجم)
        اطبع("الخط: " + هذا.الخط)
        اطبع("داكن: " + هذا.داكن)
    نهاية
نهاية

# إنشاء مع القيم الافتراضية
متغير إ = جديد إعدادات()
إ.عرض()

# تخصيص
إ.وضع_داكن()
إ.عيّن_حجم(18)
إ.عرض()`,exercise:'أنشئ صنف "طلب_طعام" بقيم افتراضية (حجم وسط، بدون إضافات، عادي) ودوال لتخصيص كل خيار.',hint:"عيّن قيم افتراضية في الباني ووفر دالة لكل تخصيص.",tips:["القيم الافتراضية تجعل الصنف سهل الاستخدام","دوال التخصيص تتيح المرونة","نمط Builder شائع في البرمجة الحديثة"],concepts:["قيم افتراضية","تخصيص","نمط البناء"]},{id:"l3_16",title:"مشروع: لعبة RPG بسيطة",desc:"تطبيق شامل يستخدم الأصناف المتقدمة: لعبة أدوار بسيطة.",explanation:"ألعاب RPG مثال مثالي للبرمجة الكائنية: كل شخصية كائن له حالة (صحة، قوة) وسلوك (هاجم، اشفِ). الكائنات تتفاعل: بطل يهاجم وحشاً، الوحش يتلقى ضرراً، صحته تنقص... هذا التفاعل الديناميكي بين الكائنات هو جوهر البرمجة الكائنية وسبب قوتها!",code:`صنف شخصية
    باني(الاسم، الصحة، القوة)
        هذا.الاسم = الاسم
        هذا.الصحة = الصحة
        هذا.القوة = القوة
        هذا.حي = صحيح
    نهاية

    دالة هاجم(هدف)
        إذا (ليس هذا.حي)
            اطبع(هذا.الاسم + " لا يستطيع الهجوم!")
            ارجع لاشيء
        نهاية
        اطبع(هذا.الاسم + " يهاجم " + هدف.الاسم + " بقوة " + هذا.القوة)
        هدف.تلقي_ضرر(هذا.القوة)
    نهاية

    دالة تلقي_ضرر(ضرر)
        هذا.الصحة -= ضرر
        إذا (هذا.الصحة <= 0)
            هذا.الصحة = 0
            هذا.حي = خطأ
            اطبع("💀 " + هذا.الاسم + " سقط!")
        وإلا
            اطبع(هذا.الاسم + " — الصحة: " + هذا.الصحة)
        نهاية
    نهاية

    دالة اشفِ(مقدار)
        إذا (هذا.حي)
            هذا.الصحة += مقدار
            اطبع("💚 " + هذا.الاسم + " تعافى! الصحة: " + هذا.الصحة)
        نهاية
    نهاية

    دالة حالة()
        متغير رمز = هذا.حي ? "❤️" : "💀"
        اطبع(رمز + " " + هذا.الاسم + " | صحة: " + هذا.الصحة + " | قوة: " + هذا.القوة)
    نهاية
نهاية

# جولة قتال!
متغير بطل = جديد شخصية("أحمد"، 100، 25)
متغير وحش = جديد شخصية("التنين"، 80، 30)

اطبع("⚔️ بداية المعركة!")
بطل.حالة()
وحش.حالة()

بطل.هاجم(وحش)
وحش.هاجم(بطل)
بطل.هاجم(وحش)
بطل.اشفِ(20)
بطل.هاجم(وحش)
بطل.هاجم(وحش)`,exercise:'أضف صنف "سلاح" (اسم، ضرر_إضافي) واجعل الشخصية تحمل سلاحاً يزيد ضررها. أضف دالة "تجهيز_سلاح(سلاح)".',hint:"في الهجوم، اجعل الضرر = هذا.القوة + هذا.السلاح.ضرر_إضافي إذا كان السلاح موجوداً.",tips:["ألعاب RPG مثال ممتاز للبرمجة الكائنية","كل كائن (شخصية) يدير حالته الخاصة","التفاعل بين الكائنات يصنع المنطق"],concepts:["لعبة","تفاعل كائنات","حالة","منطق"]}]},_c={id:"l3_inheritance",title:"الوراثة",icon:"🌳",lessons:[{id:"l3_17",title:"مفهوم الوراثة",desc:"كيف يرث صنف من صنف آخر — بناء أصناف متخصصة.",explanation:'الوراثة كالعلاقة العائلية: الابن يرث صفات الأب ويضيف صفاته الخاصة. في البرمجة، صنف "سيارة" يرث من "مركبة" — يأخذ كل خصائصها (السرعة، اللون) ويضيف الخاص به (عدد الأبواب). هذا يوفر التكرار ويبني تسلسلات منطقية بين الأصناف.',code:`# الوراثة: صنف يرث خصائص ودوال صنف آخر
# الصنف الأب (الأساس)
صنف شكل
    باني(الاسم)
        هذا.الاسم = الاسم
    نهاية

    دالة وصف()
        اطبع("شكل: " + هذا.الاسم)
    نهاية
نهاية

# الصنف الابن يرث من الأب
صنف مربع يرث شكل
    باني(الضلع)
        الأساس("مربع")
        هذا.الضلع = الضلع
    نهاية

    دالة المساحة()
        ارجع هذا.الضلع * هذا.الضلع
    نهاية
نهاية

صنف دائرة يرث شكل
    باني(نصف_القطر)
        الأساس("دائرة")
        هذا.نصف_القطر = نصف_القطر
    نهاية

    دالة المساحة()
        ارجع 3.14159 * هذا.نصف_القطر * هذا.نصف_القطر
    نهاية
نهاية

متغير م = جديد مربع(5)
م.وصف()   # موروثة من شكل!
اطبع("المساحة: " + م.المساحة())

متغير د = جديد دائرة(7)
د.وصف()
اطبع("المساحة: " + د.المساحة())`,exercise:'أنشئ صنف "مثلث" يرث من "شكل" ويحسب المساحة (القاعدة × الارتفاع / 2).',hint:'صنف مثلث يرث شكل، الباني يأخذ القاعدة والارتفاع ويستدعي الأساس("مثلث").',tips:['"يرث" تعني أن الصنف الابن يأخذ كل شيء من الأب','"الأساس()" تستدعي باني الصنف الأب',"الابن يمكنه إضافة خصائص ودوال جديدة"],concepts:["يرث","الأساس","صنف أب","صنف ابن"]},{id:"l3_18",title:"استدعاء الأساس",desc:'كيفية استدعاء باني ودوال الصنف الأب بـ "الأساس".',explanation:'"الأساس()" كأنك تتصل بوالدك — استدعاء باني الأب ليهيئ الجزء الموروث. إذا "قطة" ترث من "حيوان"، الأساس() يهيئ خصائص الحيوان (الاسم، الصوت)، ثم باني القطة يضيف الخاص بها (اللون). هذا التسلسل يضمن أن كل شيء يُهيأ صحيحاً.',code:`صنف حيوان
    باني(الاسم، الصوت)
        هذا.الاسم = الاسم
        هذا.الصوت = الصوت
    نهاية

    دالة تكلم()
        اطبع(هذا.الاسم + " يقول: " + هذا.الصوت)
    نهاية

    دالة معلومات()
        اطبع("حيوان: " + هذا.الاسم)
    نهاية
نهاية

صنف قطة يرث حيوان
    باني(الاسم، اللون)
        # استدعاء باني الأب
        الأساس(الاسم، "مياو")
        هذا.اللون = اللون
    نهاية

    دالة معلومات()
        # يمكننا تجاوز دالة الأب
        اطبع("🐱 قطة: " + هذا.الاسم + " — لون: " + هذا.اللون)
    نهاية
نهاية

صنف كلب يرث حيوان
    باني(الاسم، السلالة)
        الأساس(الاسم، "هاو هاو")
        هذا.السلالة = السلالة
    نهاية

    دالة معلومات()
        اطبع("🐕 كلب: " + هذا.الاسم + " — سلالة: " + هذا.السلالة)
    نهاية
نهاية

متغير قطتي = جديد قطة("ميمي"، "أبيض")
متغير كلبي = جديد كلب("ريكس"، "جيرمن شيبرد")

قطتي.تكلم()     # موروثة من حيوان
قطتي.معلومات()   # مُتجاوَزة

كلبي.تكلم()
كلبي.معلومات()`,exercise:'أنشئ صنف "موظف" (اسم، راتب) وصنف "مدير" يرث من موظف ويضيف "القسم" ويتجاوز دالة المعلومات.',hint:"الأساس(الاسم، الراتب) في باني المدير، ثم هذا.القسم = القسم.",tips:["الأساس() في الباني تُمرر المعاملات للأب","تجاوز الدالة = كتابة دالة بنفس الاسم في الابن","الدوال غير المُتجاوَزة تبقى متاحة"],concepts:["الأساس","تجاوز الدوال","توسيع"]},{id:"l3_19",title:"تجاوز الدوال (Override)",desc:"كتابة نسخة مخصصة من دالة الأب في الابن.",explanation:'التجاوز كالأب الذي علّم ابنه الطبخ، لكن الابن طوّر وصفته الخاصة! الابن "يتجاوز" الدالة بإعادة كتابتها بسلوك مختلف. السطح الخارجي الإسم والمعاملات نفسها، لكن التنفيذ مخصص. هذا هو "تعددية الأشكال" — نفس الواجهة، سلوك مختلف.',code:`صنف مركبة
    باني(النوع، السرعة_القصوى)
        هذا.النوع = النوع
        هذا.السرعة_القصوى = السرعة_القصوى
        هذا.السرعة_الحالية = 0
    نهاية

    دالة تسريع(مقدار)
        هذا.السرعة_الحالية += مقدار
        إذا (هذا.السرعة_الحالية > هذا.السرعة_القصوى)
            هذا.السرعة_الحالية = هذا.السرعة_القصوى
        نهاية
    نهاية

    دالة وصف()
        اطبع(هذا.النوع + " — السرعة: " + هذا.السرعة_الحالية)
    نهاية
نهاية

صنف سيارة_سباق يرث مركبة
    باني(الاسم)
        الأساس("سيارة سباق"، 300)
        هذا.الاسم_المحدد = الاسم
        هذا.توربو = خطأ
    نهاية

    # تجاوز: تسريع مضاعف
    دالة تسريع(مقدار)
        متغير مقدار_فعلي = مقدار
        إذا (هذا.توربو)
            مقدار_فعلي = مقدار * 2
            اطبع("🔥 توربو!")
        نهاية
        هذا.السرعة_الحالية += مقدار_فعلي
        إذا (هذا.السرعة_الحالية > هذا.السرعة_القصوى)
            هذا.السرعة_الحالية = هذا.السرعة_القصوى
        نهاية
    نهاية

    دالة فعّل_توربو()
        هذا.توربو = صحيح
    نهاية

    دالة وصف()
        متغير تربو_نص = هذا.توربو ? " 🔥" : ""
        اطبع("🏎️ " + هذا.الاسم_المحدد + تربو_نص + " — السرعة: " + هذا.السرعة_الحالية)
    نهاية
نهاية

متغير سباق = جديد سيارة_سباق("فيراري")
سباق.تسريع(50)
سباق.وصف()

سباق.فعّل_توربو()
سباق.تسريع(50)
سباق.وصف()`,exercise:'أنشئ صنف "طائر" يرث من "حيوان" مع دالة طيران() وتجاوز دالة تكلم() لتشمل "زقزقة".',hint:'صنف طائر يرث حيوان، الأساس(الاسم، "زقزقة")، وأضف دالة طيران() جديدة.',tips:["التجاوز يُبقي نفس اسم الدالة لكن بسلوك مختلف","الابن يمكنه إعادة تعريف أي دالة من الأب",'هذا يسمى "تعددية الأشكال" (Polymorphism)'],concepts:["تجاوز","تعددية الأشكال","سلوك مخصص"]},{id:"l3_20",title:"الوراثة المتعددة المستويات",desc:"سلاسل الوراثة: صنف يرث من صنف يرث من صنف.",explanation:"الوراثة يمكن أن تكون سلسلة: ثديي → حيوان → كائن حي. الثديي يرث من حيوان (الحركة)، الذي يرث من كائن حي (التنفس). كل مستوى يضيف تخصصاً. لكن حذار: لا تعمّق كثيراً (2-3 مستويات كافية)، وإلا صعب الفهم والصيانة!",code:`# سلسلة وراثة: كائن_حي → حيوان → ثديي
صنف كائن_حي
    باني(الاسم)
        هذا.الاسم = الاسم
        هذا.حي = صحيح
    نهاية

    دالة تنفس()
        اطبع(هذا.الاسم + " يتنفس")
    نهاية
نهاية

صنف حيوان يرث كائن_حي
    باني(الاسم، النوع)
        الأساس(الاسم)
        هذا.النوع = النوع
    نهاية

    دالة تحرك()
        اطبع(هذا.الاسم + " يتحرك")
    نهاية
نهاية

صنف ثديي يرث حيوان
    باني(الاسم، الفراء)
        الأساس(الاسم، "ثديي")
        هذا.الفراء = الفراء
    نهاية

    دالة أرضع()
        اطبع(هذا.الاسم + " يُرضع صغاره")
    نهاية
نهاية

# الثديي يرث كل شيء!
متغير أسد = جديد ثديي("الأسد"، "ذهبي")
أسد.تنفس()   # من كائن_حي
أسد.تحرك()   # من حيوان
أسد.أرضع()   # خاص بالثديي
اطبع("النوع: " + أسد.النوع)
اطبع("حي: " + أسد.حي)`,exercise:'أنشئ سلسلة: "جهاز" → "هاتف" → "هاتف_ذكي" مع خصائص ودوال في كل مستوى.',hint:"جهاز: اسم + تشغيل(). هاتف يرث جهاز: رقم + اتصل(). هاتف_ذكي يرث هاتف: تطبيقات + تصوير().",tips:["الوراثة المتعددة المستويات تبني تسلسلاً هرمياً","كل مستوى يضيف خصائص ودوال جديدة","لا تُعمّق كثيراً — 2-3 مستويات كافية عادة"],concepts:["وراثة متعددة المستويات","تسلسل هرمي"]},{id:"l3_21",title:"بناء تسلسل هرمي للأصناف",desc:"تصميم تسلسل هرمي مناسب: متى ترث ومتى تركّب.",explanation:'اسأل نفسك: الابن "نوع من" الأب؟ زر "نوع من" عنصر واجهة ✔️ — استخدم الوراثة. سيارة "تملك" محركاً (ليست "نوع من" محرك) — استخدم التركيب. القاعدة الذهبية: الوراثة لعلاقات "is-a"، التركيب لعلاقات "has-a". التصميم الجيد يجعل الكود طبيعياً ومفهوماً.',code:`# تسلسل هرمي: واجهة مستخدم
صنف عنصر_واجهة
    باني(العرض، الارتفاع)
        هذا.العرض = العرض
        هذا.الارتفاع = الارتفاع
        هذا.مرئي = صحيح
    نهاية

    دالة اظهر()
        هذا.مرئي = صحيح
    نهاية

    دالة اخفِ()
        هذا.مرئي = خطأ
    نهاية
نهاية

صنف زر يرث عنصر_واجهة
    باني(النص، العرض، الارتفاع)
        الأساس(العرض، الارتفاع)
        هذا.النص = النص
    نهاية

    دالة انقر()
        اطبع("🖱️ تم نقر الزر: " + هذا.النص)
    نهاية

    دالة ارسم()
        إذا (هذا.مرئي)
            اطبع("[" + هذا.النص + "] (" + هذا.العرض + "×" + هذا.الارتفاع + ")")
        نهاية
    نهاية
نهاية

صنف حقل_نص يرث عنصر_واجهة
    باني(التسمية، العرض)
        الأساس(العرض، 30)
        هذا.التسمية = التسمية
        هذا.القيمة = ""
    نهاية

    دالة اكتب(نص)
        هذا.القيمة += نص
    نهاية

    دالة ارسم()
        إذا (هذا.مرئي)
            اطبع(هذا.التسمية + ": [" + هذا.القيمة + "]")
        نهاية
    نهاية
نهاية

متغير زرOK = جديد زر("موافق"، 100، 40)
متغير حقل_اسم = جديد حقل_نص("الاسم"، 200)

زرOK.ارسم()
حقل_اسم.اكتب("أحمد")
حقل_اسم.ارسم()

حقل_اسم.اخفِ()  # دالة موروثة
حقل_اسم.ارسم()   # لن يظهر شيء`,exercise:'أضف صنف "مربع_اختيار" يرث من عنصر_واجهة مع حالة (مُحدد/غير محدد) ودالة بدّل() وارسم().',hint:"هذا.محدد = خطأ في الباني، دالة بدّل() تعكس القيمة، ارسم() تطبع ☑ أو ☐.",tips:['استخدم الوراثة عندما يكون الابن "نوعاً من" الأب','زر "نوع من" عنصر_واجهة ✅','سيارة ليست "نوعاً من" محرك — استخدم التركيب ❌'],concepts:["تصميم هرمي","is-a علاقة","التركيب مقابل الوراثة"]},{id:"l3_22",title:"الوراثة مع محددات الوصول",desc:"كيف تعمل محددات الوصول مع الوراثة — المحمي (محمي).",explanation:'"محمي" كأسرار العائلة: الأب يشاركها مع الأبناء، لكن ليس الغرباء. "عام" للجميع، "خاص" للصنف فقط، "محمي" للصنف والأبناء. هذا يسمح للأبناء باستخدام تفاصيل داخلية مع إخفائها عن العالم الخارجي — توازن بين المرونة والحماية.',code:`صنف حساب_مستخدم
    باني(الاسم، البريد)
        هذا.الاسم = الاسم
        هذا.البريد = البريد
        هذا.نشط = صحيح
    نهاية

    عام دالة معلومات()
        اطبع("المستخدم: " + هذا.الاسم + " — " + هذا.البريد)
    نهاية

    # محمي: يمكن للأبناء الوصول إليه
    محمي دالة تحقق_نشاط()
        ارجع هذا.نشط
    نهاية

    خاص دالة رمز_داخلي()
        ارجع "ABC123"
    نهاية
نهاية

صنف مدير_نظام يرث حساب_مستخدم
    باني(الاسم، البريد، المستوى)
        الأساس(الاسم، البريد)
        هذا.المستوى = المستوى
    نهاية

    دالة لوحة_التحكم()
        # يمكنه الوصول للمحمي
        إذا (هذا.تحقق_نشاط())
            اطبع("🔧 مرحباً بالمدير " + هذا.الاسم)
            اطبع("المستوى: " + هذا.المستوى)
        وإلا
            اطبع("الحساب معطل!")
        نهاية
    نهاية
نهاية

متغير مدير = جديد مدير_نظام("أحمد"، "admin@email.com"، "أعلى")
مدير.معلومات()      # عام ← يعمل
مدير.لوحة_التحكم()  # يستخدم المحمي داخلياً`,exercise:'أنشئ صنف "عامل" يرث من "شخص" وصنف "مهندس" يرث من "عامل". اجعل بعض الدوال محمية يستخدمها المهندس.',hint:"عامل يحتوي محمي دالة حساب_أجر(ساعات) يستخدمها المهندس في دالة الراتب().",tips:["عام: الكل يراه","محمي: الصنف والأبناء فقط","خاص: الصنف نفسه فقط"],concepts:["محمي","وراثة الوصول","إخفاء المعلومات"]},{id:"l3_23",title:"تصميم أصناف واقعية",desc:"مبادئ تصميم الأصناف الجيدة مع أمثلة واقعية.",explanation:'التصميم الجيد يعكس العالم الحقيقي: "منتج" يعرف سعره، "طلب" يحسب إجماليه، "عنصر سلة" يربطهما. المبدأ: كل صنف مسؤول عن شيء واحد (Single Responsibility). فكّر في الكائنات كأشياء حقيقية تتعاون، وسيكون تصميمك طبيعياً وسهل الفهم.',code:`# تصميم نظام متجر إلكتروني
صنف منتج
    باني(المعرف، الاسم، السعر، الكمية)
        هذا.المعرف = المعرف
        هذا.الاسم = الاسم
        هذا.السعر = السعر
        هذا.الكمية = الكمية
    نهاية

    دالة متوفر()
        ارجع هذا.الكمية > 0
    نهاية
نهاية

صنف عنصر_سلة
    باني(المنتج، العدد)
        هذا.المنتج = المنتج
        هذا.العدد = العدد
    نهاية

    دالة المجموع_الفرعي()
        ارجع هذا.المنتج.السعر * هذا.العدد
    نهاية
نهاية

صنف طلب
    باني(رقم_الطلب)
        هذا.رقم = رقم_الطلب
        هذا.العناصر = []
        هذا.الحالة = "جديد"
    نهاية

    دالة أضف_عنصر(منتج، عدد)
        إذا (ليس منتج.متوفر())
            اطبع("❌ " + منتج.الاسم + " غير متوفر!")
            ارجع لاشيء
        نهاية
        هذا.العناصر.أضف(جديد عنصر_سلة(منتج، عدد))
        اطبع("✅ تمت إضافة " + عدد + " × " + منتج.الاسم)
    نهاية

    دالة الإجمالي()
        متغير مجموع = 0
        لكل ع في هذا.العناصر
            مجموع += ع.المجموع_الفرعي()
        نهاية
        ارجع مجموع
    نهاية

    دالة فاتورة()
        اطبع("\\n🧾 فاتورة طلب #" + هذا.رقم)
        لكل ع في هذا.العناصر
            اطبع("  " + ع.المنتج.الاسم + " × " + ع.العدد + " = " + ع.المجموع_الفرعي())
        نهاية
        اطبع("  الإجمالي: " + هذا.الإجمالي() + " ريال")
    نهاية
نهاية

متغير لابتوب = جديد منتج(1، "لابتوب"، 3500، 10)
متغير ماوس = جديد منتج(2، "ماوس"، 120، 50)
متغير شاشة = جديد منتج(3، "شاشة"، 1200، 5)

متغير طلبي = جديد طلب(1001)
طلبي.أضف_عنصر(لابتوب، 1)
طلبي.أضف_عنصر(ماوس، 2)
طلبي.أضف_عنصر(شاشة، 1)
طلبي.فاتورة()`,exercise:'أضف صنف "عميل" مع اسم وعنوان ومصفوفة طلبات، ودالة إجمالي_المشتريات() تحسب مجموع كل الطلبات.',hint:"دالة إجمالي_المشتريات() تستخدم حلقة على المصفوفة وتستدعي طلب.الإجمالي() لكل طلب.",tips:["كل صنف مسؤول عن شيء واحد","الأصناف تتعاون معاً لبناء النظام","فكر في الكائنات كأشياء حقيقية"],concepts:["تصميم واقعي","فصل المسؤوليات","تعاون"]},{id:"l3_24",title:"مشروع: نظام المدرسة",desc:"مشروع شامل: نظام إدارة مدرسة يستخدم الوراثة.",explanation:'هذا المشروع يطبق كل ما تعلمته: "شخص" أساس، "طالب" و"معلم" يرثون منه، ولكل منهم خصائص ودوال مخصصة. لاحظ التفاعل: المعلم يعطي درجة للطالب، الطالب يحفظها ويحسب معدله. الكائنات تتعاون لإنجاز عمل حقيقي!',code:`# نظام المدرسة
صنف شخص
    باني(الاسم، العمر)
        هذا.الاسم = الاسم
        هذا.العمر = العمر
    نهاية
نهاية

صنف طالب يرث شخص
    باني(الاسم، العمر، الصف)
        الأساس(الاسم، العمر)
        هذا.الصف = الصف
        هذا.الدرجات = {}
    نهاية

    دالة أضف_درجة(المادة، الدرجة)
        هذا.الدرجات[المادة] = الدرجة
    نهاية

    دالة المعدل()
        متغير مجموع = 0
        متغير عدد = 0
        لكل مادة في هذا.الدرجات.مفاتيح()
            مجموع += هذا.الدرجات[مادة]
            عدد += 1
        نهاية
        إذا (عدد == 0)
            ارجع 0
        نهاية
        ارجع مجموع / عدد
    نهاية

    دالة تقرير()
        اطبع("📝 " + هذا.الاسم + " — الصف " + هذا.الصف)
        لكل مادة في هذا.الدرجات.مفاتيح()
            اطبع("  " + مادة + ": " + هذا.الدرجات[مادة])
        نهاية
        اطبع("  المعدل: " + هذا.المعدل())
    نهاية
نهاية

صنف معلم يرث شخص
    باني(الاسم، العمر، المادة)
        الأساس(الاسم، العمر)
        هذا.المادة = المادة
        هذا.الطلاب = []
    نهاية

    دالة أضف_طالب(طالب)
        هذا.الطلاب.أضف(طالب)
    نهاية

    دالة أعطِ_درجة(طالب، درجة)
        طالب.أضف_درجة(هذا.المادة، درجة)
        اطبع(هذا.الاسم + " أعطى " + طالب.الاسم + " درجة " + درجة + " في " + هذا.المادة)
    نهاية
نهاية

# التجربة
متغير ط1 = جديد طالب("أحمد"، 15، "الثالث")
متغير ط2 = جديد طالب("سارة"، 14، "الثالث")

متغير أستاذ_رياضيات = جديد معلم("خالد"، 40، "الرياضيات")
متغير أستاذ_علوم = جديد معلم("نورة"، 35، "العلوم")

أستاذ_رياضيات.أعطِ_درجة(ط1، 90)
أستاذ_علوم.أعطِ_درجة(ط1، 85)
أستاذ_رياضيات.أعطِ_درجة(ط2، 95)
أستاذ_علوم.أعطِ_درجة(ط2، 92)

ط1.تقرير()
ط2.تقرير()`,exercise:'أضف صنف "فصل" يحتوي على مصفوفة طلاب ومعلم، مع دوال: أعلى_طالب() ومتوسط_الفصل().',hint:"في أعلى_طالب() قارن المعدلات، في متوسط_الفصل() اجمع كل المعدلات واقسم.",tips:["التسلسل: شخص → طالب/معلم — وراثة طبيعية","طالب ومعلم نوعان من شخص","الكائنات تتفاعل — المعلم يعطي درجة للطالب"],concepts:["مشروع وراثة","تسلسل هرمي","تفاعل أصناف"]}]},uc={id:"l3_structs_enums",title:"البنيات والتعدادات",icon:"📦",lessons:[{id:"l3_25",title:"البنيات (بنية)",desc:"البنيات — حاويات بيانات بسيطة بدون دوال معقدة.",explanation:"البنية كصندوق بريد: مساحات محددة للاسم والعنوان، بدون سلوك. الفرق عن الصنف: البنية لتجميع بيانات فقط، الصنف لبيانات + سلوك. استخدم بنية لأشياء كالإحداثيات (س، ص) أو التواريخ (يوم، شهر، سنة) — بيانات بسيطة مترابطة.",code:`# البنية: مثل الصنف لكن أبسط — لتجميع البيانات
بنية نقطة
    س
    ص
نهاية

# إنشاء بنية مثل الصنف
متغير ن1 = جديد نقطة(10، 20)
متغير ن2 = جديد نقطة(30، 40)

اطبع("نقطة 1: (" + ن1.س + "، " + ن1.ص + ")")
اطبع("نقطة 2: (" + ن2.س + "، " + ن2.ص + ")")

# البنية قابلة للتعديل
ن1.س = 100
اطبع("نقطة 1 بعد التعديل: (" + ن1.س + "، " + ن1.ص + ")")

# بنية أخرى
بنية مستطيل
    العرض
    الارتفاع
نهاية

متغير م = جديد مستطيل(50، 30)
اطبع("المساحة: " + م.العرض * م.الارتفاع)`,exercise:'أنشئ بنية "لون" مع قيم (أحمر، أخضر، أزرق) ثم أنشئ عدة ألوان واطبعها.',hint:"بنية لون  أحمر  أخضر  أزرق  نهاية ثم جديد لون(255, 0, 0) للأحمر.",tips:["البنيات مثالية لتجميع بيانات بسيطة","الفرق عن الصنف: بنية = بيانات فقط","الحقول تُعرّف بأسمائها مباشرة"],concepts:["بنية","حقول","تجميع بيانات"]},{id:"l3_26",title:"استخدامات البنيات",desc:"أمثلة عملية على استخدام البنيات في البرامج.",explanation:"البنيات في كل مكان: إحداثيات GPS (خط الطول، خط العرض)، تاريخ (يوم، شهر، سنة)، عنوان (مدينة، حي، شارع). خفيفة وسريعة — لا باني معقد. يمكن تخزينها في مصفوفات: مصفوفة إحداثيات لرسم خريطة، مصفوفة تواريخ لجدول مواعيد.",code:`# بنية لتمثيل إحداثيات GPS
بنية إحداثية
    خط_العرض
    خط_الطول
نهاية

# بنية لتمثيل عنوان
بنية عنوان
    المدينة
    الحي
    الشارع
    الرقم
نهاية

# بنية لتمثيل تاريخ
بنية تاريخ
    اليوم
    الشهر
    السنة
نهاية

# استخدام البنيات
متغير الموقع = جديد إحداثية(24.7136، 46.6753)
اطبع("الموقع: " + الموقع.خط_العرض + "، " + الموقع.خط_الطول)

متغير عنواني = جديد عنوان("الرياض"، "العليا"، "الملك فهد"، 42)
اطبع("العنوان: " + عنواني.الحي + "، " + عنواني.الشارع)

متغير اليوم = جديد تاريخ(15، 6، 2025)
اطبع("التاريخ: " + اليوم.اليوم + "/" + اليوم.الشهر + "/" + اليوم.السنة)

# مصفوفة من البنيات
متغير مواقع = [
    جديد إحداثية(24.7136، 46.6753)،
    جديد إحداثية(21.4225، 39.8262)،
    جديد إحداثية(26.3927، 49.9777)
]

لكل م في مواقع
    اطبع("📍 " + م.خط_العرض + "، " + م.خط_الطول)
نهاية`,exercise:'أنشئ بنية "طالب_بيانات" (اسم، عمر، معدل) ومصفوفة من 3 طلاب، ثم ابحث عن الطالب الأعلى معدلاً.',hint:"استخدم حلقة وقارن المعدلات لإيجاد الأعلى.",tips:["البنيات أخف من الأصناف — لا باني معقد ولا دوال","استخدم البنيات للبيانات البسيطة المترابطة","يمكن تخزين البنيات في مصفوفات وخرائط"],concepts:["بنيات عملية","مصفوفة بنيات"]},{id:"l3_27",title:"التعدادات (تعداد)",desc:"التعدادات — تعريف مجموعة محدودة من القيم المسماة.",explanation:'التعداد كقائمة اختيارات محددة: أيام الأسبوع (7 قيم فقط)، حالات الطلب (جديد، قيد_التجهيز، شُحن، تم). الفائدة: بدلاً من أرقام غامضة (ما معنى 3؟)، تستخدم أسماء واضحة ("يوم.الجمعة"). يمنع الأخطاء — لا يمكنك كتابة قيمة خاطئة!',code:`# التعداد: مجموعة قيم ثابتة مسماة
تعداد يوم
    السبت
    الأحد
    الاثنين
    الثلاثاء
    الأربعاء
    الخميس
    الجمعة
نهاية

# القيم أرقام تبدأ من 0
اطبع(يوم.السبت)     # 0
اطبع(يوم.الأحد)     # 1
اطبع(يوم.الجمعة)    # 6

# المقارنة
متغير اليوم = يوم.الخميس
إذا (اليوم == يوم.الخميس)
    اطبع("اليوم هو الخميس!")
نهاية

إذا (اليوم == يوم.الجمعة أو اليوم == يوم.السبت)
    اطبع("عطلة نهاية الأسبوع")
وإلا
    اطبع("يوم عمل")
نهاية

# تعداد آخر
تعداد حالة_الطلب
    جديد
    قيد_التجهيز
    شُحن
    تم_التسليم
    ملغي
نهاية

متغير طلبي = حالة_الطلب.قيد_التجهيز
اطبع("حالة الطلب: " + طلبي)`,exercise:'أنشئ تعداد "فصل" (شتاء، ربيع، صيف، خريف) واكتب دالة تأخذ فصلاً وتطبع نصيحة مناسبة.',hint:"دالة نصيحة_الفصل(ف) مع إذا/وإلا لكل قيمة من التعداد.",tips:["التعدادات تمنع الأخطاء — لا يمكنك كتابة قيمة خاطئة","القيم تبدأ من 0 افتراضياً","استخدم التعدادات بدلاً من أرقام أو نصوص عشوائية"],concepts:["تعداد","قيم مسماة","ثوابت"]},{id:"l3_28",title:"التعدادات مع الشروط",desc:"استخدام التعدادات في الشروط والمطابقة.",explanation:'التعدادات توضح الشروط: "إذا (المستوى == مستوى_الخطورة.حرج)" أوضح من "إذا (المستوى == 3)". التعدادات أرقام (0، 1، 2...) فيمكنك مقارنتها بـ ==، <، >. مثال: "إذا (النوع >= نوع_المستخدم.مشرف)" لفحص الصلاحيات بطريقة أنيقة.',code:`تعداد مستوى_الخطورة
    منخفض
    متوسط
    عالي
    حرج
نهاية

دالة معالجة_تنبيه(المستوى، الرسالة)
    إذا (المستوى == مستوى_الخطورة.حرج)
        اطبع("🔴 حرج: " + الرسالة)
        اطبع("   → إرسال إشعار فوري!")
    وإلا
        إذا (المستوى == مستوى_الخطورة.عالي)
            اطبع("🟠 عالي: " + الرسالة)
        وإلا
            إذا (المستوى == مستوى_الخطورة.متوسط)
                اطبع("🟡 متوسط: " + الرسالة)
            وإلا
                اطبع("🟢 منخفض: " + الرسالة)
            نهاية
        نهاية
    نهاية
نهاية

تعداد نوع_المستخدم
    زائر
    عضو
    مشرف
    مدير
نهاية

دالة صلاحيات(النوع)
    إذا (النوع >= نوع_المستخدم.مشرف)
        اطبع("✅ صلاحية: حذف المحتوى")
    نهاية
    إذا (النوع >= نوع_المستخدم.عضو)
        اطبع("✅ صلاحية: إضافة تعليق")
    نهاية
    اطبع("✅ صلاحية: القراءة")
نهاية

معالجة_تنبيه(مستوى_الخطورة.حرج، "القرص ممتلئ!")
معالجة_تنبيه(مستوى_الخطورة.منخفض، "ذاكرة مؤقتة قديمة")

اطبع("\\n--- صلاحيات المشرف ---")
صلاحيات(نوع_المستخدم.مشرف)`,exercise:'أنشئ تعداد "حالة_اللاعب" (عادي، مسموم، محترق، مجمد) ودالة تطبق التأثير على صحة اللاعب.',hint:"كل حالة تؤثر بشكل مختلف: مسموم يطرح 5، محترق يطرح 10، مجمد يمنع الحركة.",tips:["التعدادات أرقام — يمكن مقارنتها بـ ==، <، >","مفيدة جداً لحالات الأنظمة (جديد، قيد_التنفيذ، مكتمل)","أوضح من استخدام أرقام مباشرة (0، 1، 2)"],concepts:["تعداد مع شروط","حالات النظام","مقارنة"]},{id:"l3_29",title:"البنيات مع الأصناف",desc:"دمج البنيات مع الأصناف لتنظيم البيانات والسلوك.",explanation:"المزيج المثالي: البنيات للبيانات المترابطة (موقع، حجم)، الأصناف للسلوك (عنصر_رسومي يتحرك ويتغير). تخيّل زراً على الشاشة: موقعه (بنية) + حجمه (بنية) + سلوكه كالنقر والرسم (دوال الصنف). فصل المسؤوليات يعني كود أنظف!",code:`# البنية للبيانات، الصنف للسلوك
بنية موقع
    س
    ص
نهاية

بنية حجم
    العرض
    الارتفاع
نهاية

تعداد نوع_الشكل
    مربع
    دائرة
    مثلث
نهاية

صنف عنصر_رسومي
    باني(الاسم، النوع، الموقع، الحجم)
        هذا.الاسم = الاسم
        هذا.النوع = النوع
        هذا.الموقع = الموقع
        هذا.الحجم = الحجم
        هذا.مرئي = صحيح
    نهاية

    دالة انقل(س_جديد، ص_جديد)
        هذا.الموقع = جديد موقع(س_جديد، ص_جديد)
    نهاية

    دالة غيّر_الحجم(عرض، ارتفاع)
        هذا.الحجم = جديد حجم(عرض، ارتفاع)
    نهاية

    دالة وصف()
        اطبع(هذا.الاسم + " عند (" + هذا.الموقع.س + "،" + هذا.الموقع.ص + ")" +
              " حجم: " + هذا.الحجم.العرض + "×" + هذا.الحجم.الارتفاع)
    نهاية
نهاية

متغير زر = جديد عنصر_رسومي(
    "زر_حفظ"،
    نوع_الشكل.مربع،
    جديد موقع(100، 200)،
    جديد حجم(120، 40)
)

زر.وصف()
زر.انقل(150، 250)
زر.وصف()`,exercise:'أنشئ بنية "لون_RGB" ثم أضفها لعنصر_رسومي كخاصية إضافية مع دالة لتغيير اللون.',hint:"أضف بنية لون_RGB (أحمر، أخضر، أزرق) واستخدمها كمعامل إضافي في الباني.",tips:["البنيات للبيانات المترابطة (موقع = س، ص)","الأصناف للسلوك المعقد (عنصر = بيانات + دوال)","التعدادات للخيارات المحدودة (نوع_الشكل)"],concepts:["بنية + صنف","فصل البيانات عن السلوك"]},{id:"l3_30",title:"التعدادات مع طابق",desc:"استخدام مطابقة الأنماط (طابق/عندما) مع التعدادات.",explanation:'التعدادات + طابق = كود أنيق! بدلاً من سلسلة إذا/وإلا، "طابق (العملية) عندما 0: جمع عندما 1: طرح..." كل قيمة تعداد لها سلوكها. "افتراضي" يلتقط أي قيمة غير متوقعة — شبكة أمان. هذا النمط شائع في الآلات الحاسبة ومعالجات الأوامر.',code:`تعداد عملية
    جمع
    طرح
    ضرب
    قسمة
نهاية

دالة حاسبة(العملية، أ، ب)
    طابق (العملية)
        عندما 0:
            ارجع أ + ب
        عندما 1:
            ارجع أ - ب
        عندما 2:
            ارجع أ * ب
        عندما 3:
            إذا (ب == 0)
                اطبع("خطأ: القسمة على صفر!")
                ارجع 0
            نهاية
            ارجع أ / ب
        افتراضي:
            اطبع("عملية غير معروفة!")
            ارجع 0
    نهاية
نهاية

اطبع("10 + 3 = " + حاسبة(عملية.جمع، 10، 3))
اطبع("10 - 3 = " + حاسبة(عملية.طرح، 10، 3))
اطبع("10 * 3 = " + حاسبة(عملية.ضرب، 10، 3))
اطبع("10 / 3 = " + حاسبة(عملية.قسمة، 10، 3))

تعداد اتجاه
    شمال
    جنوب
    شرق
    غرب
نهاية

دالة تحرك(الاتجاه، خطوات)
    طابق (الاتجاه)
        عندما 0:
            اطبع("⬆️ تحرك " + خطوات + " شمالاً")
        عندما 1:
            اطبع("⬇️ تحرك " + خطوات + " جنوباً")
        عندما 2:
            اطبع("➡️ تحرك " + خطوات + " شرقاً")
        عندما 3:
            اطبع("⬅️ تحرك " + خطوات + " غرباً")
    نهاية
نهاية

تحرك(اتجاه.شمال، 5)
تحرك(اتجاه.غرب، 3)`,exercise:'أنشئ تعداد "أمر" (تقدم، تراجع، استدر_يمين، استدر_يسار، توقف) ودالة تنفذ الأوامر على روبوت.',hint:"استخدم طابق مع قيم التعداد الرقمية (0، 1، 2، ...) لتنفيذ كل أمر.",tips:["التعدادات + طابق = كود نظيف ومقروء","كل قيمة تعداد لها رقم (0، 1، 2، ...)",'استخدم "افتراضي" للتعامل مع القيم غير المتوقعة'],concepts:["تعداد + طابق","حالات محددة"]},{id:"l3_31",title:"مشروع: نظام إدارة المخزون",desc:"مشروع يجمع البنيات والتعدادات والأصناف معاً.",explanation:"هذا المشروع يجمع كل شيء: بنية لمعلومات السعر (سعر + ضريبة)، تعدادات للفئات والحالات، وصنف للمنتج مع سلوكه. لاحظ كيف كل أداة تُستخدم في مكانها الصحيح: البنية للبيانات البسيطة، التعداد للخيارات المحدودة، الصنف للمنطق المعقد.",code:`# نظام إدارة مخزون
تعداد فئة_المنتج
    إلكترونيات
    ملابس
    طعام
    كتب
نهاية

تعداد حالة_المنتج
    متوفر
    نفذ
    قريباً
نهاية

بنية معلومات_سعر
    السعر
    الضريبة
نهاية

صنف منتج_مخزون
    باني(المعرف، الاسم، الفئة، الكمية، السعر)
        هذا.المعرف = المعرف
        هذا.الاسم = الاسم
        هذا.الفئة = الفئة
        هذا.الكمية = الكمية
        هذا.السعر_المعلومات = جديد معلومات_سعر(السعر، السعر * 0.15)
    نهاية

    دالة الحالة()
        إذا (هذا.الكمية > 10)
            ارجع حالة_المنتج.متوفر
        وإلا
            إذا (هذا.الكمية > 0)
                ارجع حالة_المنتج.قريباً
            وإلا
                ارجع حالة_المنتج.نفذ
            نهاية
        نهاية
    نهاية

    دالة السعر_الكلي()
        ارجع هذا.السعر_المعلومات.السعر + هذا.السعر_المعلومات.الضريبة
    نهاية

    دالة عرض()
        متغير حالة_نص = ""
        طابق (هذا.الحالة())
            عندما 0:
                حالة_نص = "✅ متوفر"
            عندما 1:
                حالة_نص = "❌ نفذ"
            عندما 2:
                حالة_نص = "⚠️ كمية قليلة"
        نهاية
        اطبع(هذا.المعرف + " | " + هذا.الاسم + " | " + حالة_نص +
              " | الكمية: " + هذا.الكمية +
              " | السعر: " + هذا.السعر_الكلي())
    نهاية
نهاية

# إنشاء منتجات
متغير منتجات = [
    جديد منتج_مخزون(1، "لابتوب"، فئة_المنتج.إلكترونيات، 25، 3500)،
    جديد منتج_مخزون(2، "قميص"، فئة_المنتج.ملابس، 5، 120)،
    جديد منتج_مخزون(3، "أرز"، فئة_المنتج.طعام، 0، 45)،
    جديد منتج_مخزون(4، "رواية"، فئة_المنتج.كتب، 50، 65)
]

اطبع("📦 تقرير المخزون:")
لكل م في منتجات
    م.عرض()
نهاية`,exercise:'أضف دالة "تنبيهات_المخزون" تُرجع مصفوفة بالمنتجات التي حالتها "نفذ" أو "كمية قليلة".',hint:"استخدم حلقة على المنتجات وتحقق من الحالة() ثم أضف المطابقة لمصفوفة النتائج.",tips:["البنيات للبيانات البسيطة (معلومات_سعر)","التعدادات للحالات الثابتة (فئة، حالة)","الأصناف للسلوك المعقد (منتج_مخزون)"],concepts:["مشروع متكامل","بنية + تعداد + صنف"]}]},pc={id:"l3_pattern_matching",title:"مطابقة الأنماط",icon:"🎯",lessons:[{id:"l3_32",title:"مقدمة في طابق/عندما",desc:"مطابقة الأنماط — بديل أنيق لسلاسل إذا/وإلا.",explanation:'"طابق" كمقارنة بصمة إصبع مع قاعدة بيانات: تفحص القيمة وتجد النمط المطابق. بدلاً من: "إذا == 0... وإلا إذا == 1... وإلا ..."، اكتب: "طابق (قيمة) عندما 0: ... عندما 1: ...". أوضح، أقل خطأ، والمترجم يحذرك إن نسيت حالة!',code:`# طابق: مقارنة قيمة مع عدة أنماط
متغير الدرجة = 85

طابق (الدرجة)
    عندما 100:
        اطبع("ممتاز! درجة كاملة")
    عندما 90:
        اطبع("ممتاز")
    عندما 80:
        اطبع("جيد جداً")
    عندما 70:
        اطبع("جيد")
    افتراضي:
        اطبع("درجة: " + الدرجة)
نهاية

# مثال آخر مع نصوص
متغير اللغة = "عربي"

طابق (اللغة)
    عندما "عربي":
        اطبع("مرحباً!")
    عندما "إنجليزي":
        اطبع("Hello!")
    عندما "فرنسي":
        اطبع("Bonjour!")
    افتراضي:
        اطبع("لغة غير مدعومة")
نهاية`,exercise:'اكتب دالة "اسم_الشهر" تأخذ رقم (1-12) وترجع اسم الشهر بالعربية باستخدام طابق.',hint:'طابق (الرقم) عندما 1: ارجع "يناير" عندما 2: ارجع "فبراير" ...',tips:['"طابق" أوضح من سلاسل إذا/وإلا الطويلة','"عندما" لكل نمط تريد مطابقته','"افتراضي" يلتقط أي قيمة غير مطابقة'],concepts:["طابق","عندما","افتراضي"]},{id:"l3_33",title:"مطابقة النطاقات",desc:"مطابقة قيمة ضمن نطاق من الأرقام.",explanation:'النطاقات توفر تصنيف الأرقام: "عندما 0..13: طفل" أفضل من "عندما 0 || 1 || 2 || 3...". تخيّل جدول تقييم الدرجات: 90-100 ممتاز، 80-90 جيد جداً... الصيغة "بداية..نهاية" تشمل البداية ولا تشمل النهاية. طريقة طبيعية ومقروءة جداً!',code:`# مطابقة نطاقات: عندما بداية..نهاية
متغير العمر = 25

طابق (العمر)
    عندما 0..3:
        اطبع("رضيع 👶")
    عندما 3..13:
        اطبع("طفل 🧒")
    عندما 13..18:
        اطبع("مراهق 🧑")
    عندما 18..30:
        اطبع("شاب 💪")
    عندما 30..60:
        اطبع("بالغ 🧔")
    افتراضي:
        اطبع("كبير في السن 👴")
نهاية

# استخدام عملي: تقييم الدرجات
دالة تقيّم(الدرجة)
    طابق (الدرجة)
        عندما 90..101:
            ارجع "أ+ ممتاز"
        عندما 80..90:
            ارجع "ب+ جيد جداً"
        عندما 70..80:
            ارجع "ج+ جيد"
        عندما 60..70:
            ارجع "د+ مقبول"
        عندما 0..60:
            ارجع "راسب"
        افتراضي:
            ارجع "درجة غير صالحة"
    نهاية
نهاية

لكل د في [95، 82، 73، 61، 45]
    اطبع(د + " → " + تقيّم(د))
نهاية`,exercise:"اكتب دالة تأخذ درجة الحرارة وتُرجع نصيحة ملابس مناسبة باستخدام نطاقات.",hint:"طابق مع نطاقات: 0..10 = ملابس ثقيلة، 10..20 = جاكيت، 20..35 = خفيفة، 35..50 = حار جداً.",tips:["النطاق يكتب: عندما بداية..نهاية","النطاق يشمل البداية ولا يشمل النهاية","مثالي لتصنيف الأرقام في فئات"],concepts:["نطاقات","تصنيف","فئات"]},{id:"l3_34",title:"مطابقة البدائل (OR)",desc:"مطابقة قيمة واحدة ضمن عدة خيارات.",explanation:'أحياناً عدة قيم تؤدي لنفس النتيجة: "عندما 0 || 6: عطلة". بدلاً من تكرار نفس الكود، اجمع البدائل بـ ||. مثال: "عندما "a" || "e" || "i" || "o" || "u": حرف علة". هذا يجعل الكود أقصر وأوضح — كل البدائل مرئية في سطر واحد.',code:`# مطابقة OR: عندما قيمة1 || قيمة2
متغير اليوم = 5  # الخميس

طابق (اليوم)
    عندما 0 || 6:
        اطبع("عطلة نهاية الأسبوع 🎉")
    عندما 1 || 2 || 3 || 4:
        اطبع("يوم عمل 💼")
    عندما 5:
        اطبع("الخميس — نصف يوم 🌤️")
    افتراضي:
        اطبع("يوم غير صالح")
نهاية

# مثال آخر: تصنيف الأحرف
دالة صنّف_حرف(حرف)
    طابق (حرف)
        عندما "a" || "e" || "i" || "o" || "u":
            ارجع "حرف علة"
        عندما " " || "\\t" || "\\n":
            ارجع "مسافة"
        عندما "." || "!" || "؟" || "،":
            ارجع "علامة ترقيم"
        افتراضي:
            ارجع "حرف ساكن"
    نهاية
نهاية

لكل ح في ["a"، "b"، "e"، "."، " "]
    اطبع(ح + " → " + صنّف_حرف(ح))
نهاية

# مطابقة الأوامر
متغير الأمر = "خروج"
طابق (الأمر)
    عندما "خروج" || "exit" || "quit" || "ق":
        اطبع("وداعاً! 👋")
    عندما "مساعدة" || "help" || "م":
        اطبع("الأوامر المتاحة: ...")
    افتراضي:
        اطبع("أمر غير معروف: " + الأمر)
نهاية`,exercise:"اكتب دالة تأخذ اسم فاكهة وترجع فصل الحصاد، مع تجميع عدة فواكه في نفس الفصل.",hint:'عندما "تفاح" || "برتقال": ارجع "شتاء"، عندما "بطيخ" || "مانجو": ارجع "صيف".',tips:['|| تعني "أو" — أي قيمة تطابق تكفي',"يمكن دمج عدة قيم في عندما واحد","أنظف من كتابة عدة إذا/وإلا"],concepts:["مطابقة OR","بدائل","تجميع"]},{id:"l3_35",title:"تفكيك المصفوفات",desc:"استخراج عناصر المصفوفة مباشرة في المطابقة.",explanation:'التفكيك يفتح العلبة ويستخرج ما بداخلها: "عندما [س، ص]: ..." يضع العنصر الأول في س والثاني في ص. يمكنك مزج الثوابت والمتغيرات: "عندما [0، 0]: نقطة الأصل" أو "عندما [0، ص]: على المحور العمودي". طريقة قوية لمعالجة البيانات المركبة!',code:`# تفكيك المصفوفة في طابق
متغير إحداثية = [3، 4]

طابق (إحداثية)
    عندما [0، 0]:
        اطبع("نقطة الأصل")
    عندما [0، ص]:
        اطبع("على المحور العمودي، ص = " + ص)
    عندما [س، 0]:
        اطبع("على المحور الأفقي، س = " + س)
    عندما [س، ص]:
        اطبع("نقطة عامة: (" + س + "، " + ص + ")")
نهاية

# مع مصفوفات أطول
متغير درجات = [90، 85، 78]

طابق (درجات)
    عندما []:
        اطبع("لا توجد درجات")
    عندما [د]:
        اطبع("درجة واحدة: " + د)
    عندما [أ، ب]:
        اطبع("درجتان: " + أ + " و " + ب)
    عندما [أ، ب، ج]:
        اطبع("ثلاث درجات: " + أ + "، " + ب + "، " + ج)
    افتراضي:
        اطبع("أكثر من 3 درجات")
نهاية

# استخدام عملي
دالة وصف_قائمة(قائمة)
    طابق (قائمة)
        عندما []:
            ارجع "قائمة فارغة"
        عندما [عنصر]:
            ارجع "عنصر واحد: " + عنصر
        عندما [أول، ثاني]:
            ارجع "عنصران: " + أول + " و " + ثاني
        افتراضي:
            ارجع "قائمة بها " + طول(قائمة) + " عناصر"
    نهاية
نهاية

اطبع(وصف_قائمة([]))
اطبع(وصف_قائمة(["أحمد"]))
اطبع(وصف_قائمة(["أحمد"، "سارة"]))
اطبع(وصف_قائمة(["أحمد"، "سارة"، "خالد"، "نورة"]))`,exercise:"اكتب دالة تأخذ مصفوفة [عملية، رقم1، رقم2] وتنفذ العملية الحسابية.",hint:'طابق مع عندما ["جمع"، أ، ب]: ارجع أ + ب وهكذا لباقي العمليات.',tips:["التفكيك يستخرج العناصر إلى متغيرات","يمكن مطابقة أجزاء ثابتة وأخرى متغيرة","مصفوفة فارغة تُطابق بـ []"],concepts:["تفكيك","استخراج عناصر","أنماط مصفوفات"]},{id:"l3_36",title:"أنماط متقدمة",desc:"دمج أنواع الأنماط المختلفة للمطابقة المعقدة.",explanation:'قوة المطابقة في الدمج! يمكنك مطابقة نطاق + تفكيك + بدائل معاً. الترتيب مهم: الأنماط تُفحص بالتسلسل، فضع الأكثر تحديداً أولاً. مثلاً: "عندما 100" قبل "عندما 90..101" قبل "افتراضي". هذا يتيح تعبيرات قوية ومقروءة.',code:`# دمج أنماط متعددة
دالة حلل_الإدخال(قيمة)
    طابق (قيمة)
        عندما 0:
            اطبع("صفر")
        عندما 1..100:
            اطبع("رقم صغير: " + قيمة)
        عندما 100..1000:
            اطبع("رقم متوسط: " + قيمة)
        افتراضي:
            اطبع("رقم كبير: " + قيمة)
    نهاية
نهاية

لكل ق في [0، 42، 500، 9999]
    حلل_الإدخال(ق)
نهاية

# مطابقة معقدة مع إحداثيات
دالة اتجاه_الحركة(من، إلى)
    متغير فرق_س = إلى[0] - من[0]
    متغير فرق_ص = إلى[1] - من[1]

    طابق ([فرق_س > 0 ? 1 : (فرق_س < 0 ? -1 : 0)، فرق_ص > 0 ? 1 : (فرق_ص < 0 ? -1 : 0)])
        عندما [0، 0]:
            اطبع("لا حركة")
        عندما [1، 0]:
            اطبع("→ شرق")
        عندما [-1، 0]:
            اطبع("← غرب")
        عندما [0، 1]:
            اطبع("↑ شمال")
        عندما [0، -1]:
            اطبع("↓ جنوب")
        افتراضي:
            اطبع("↗ حركة قطرية")
    نهاية
نهاية

اتجاه_الحركة([0،0]، [5،0])   # شرق
اتجاه_الحركة([0،0]، [0،3])   # شمال
اتجاه_الحركة([0،0]، [-2،0])  # غرب
اتجاه_الحركة([0،0]، [3،4])   # قطري`,exercise:"اكتب دالة تحلل استجابة HTTP (كود_الحالة) باستخدام نطاقات: 200-299 نجاح، 300-399 إعادة توجيه، 400-499 خطأ عميل، 500-599 خطأ خادم.",hint:'طابق (الكود) عندما 200..300: "نجاح" عندما 300..400: "إعادة توجيه" ...',tips:["يمكن دمج نمط حرفي + نطاق + OR + تفكيك","الأنماط تُفحص بالترتيب — ضع الأكثر تحديداً أولاً","يمكن بناء تعبيرات معقدة كمعامل لطابق"],concepts:["أنماط مركبة","ترتيب المطابقة"]},{id:"l3_37",title:"طابق مع الأصناف",desc:"استخدام مطابقة الأنماط مع خصائص الكائنات.",explanation:'لمطابقة كائن، طابق خاصيته: "طابق (شكل.النوع) عندما 0: دائرة...". التعدادات + طابق مزيج قوي لأن قيم التعداد أرقام واضحة. هذا النمط شائع جداً في بناء أنظمة الأوامر — "طابق (أمر.النوع) عندما 0: أضف عندما 1: احذف...".',code:`# مطابقة الأنماط مع الخصائص
تعداد نوع_الشكل
    دائرة
    مربع
    مثلث
نهاية

صنف شكل
    باني(النوع، الأبعاد)
        هذا.النوع = النوع
        هذا.الأبعاد = الأبعاد  # مصفوفة
    نهاية
نهاية

دالة مساحة(شكل)
    طابق (شكل.النوع)
        عندما 0:  # دائرة — نصف القطر
            ارجع 3.14159 * شكل.الأبعاد[0] * شكل.الأبعاد[0]
        عندما 1:  # مربع — الضلع
            ارجع شكل.الأبعاد[0] * شكل.الأبعاد[0]
        عندما 2:  # مثلث — قاعدة وارتفاع
            ارجع شكل.الأبعاد[0] * شكل.الأبعاد[1] / 2
        افتراضي:
            ارجع 0
    نهاية
نهاية

دالة وصف_شكل(شكل)
    طابق (شكل.النوع)
        عندما 0:
            اطبع("⭕ دائرة بنصف قطر " + شكل.الأبعاد[0])
        عندما 1:
            اطبع("🔲 مربع بضلع " + شكل.الأبعاد[0])
        عندما 2:
            اطبع("🔺 مثلث بقاعدة " + شكل.الأبعاد[0] + " وارتفاع " + شكل.الأبعاد[1])
    نهاية
نهاية

متغير أشكال = [
    جديد شكل(نوع_الشكل.دائرة، [5])،
    جديد شكل(نوع_الشكل.مربع، [4])،
    جديد شكل(نوع_الشكل.مثلث، [6، 3])
]

لكل ش في أشكال
    وصف_شكل(ش)
    اطبع("  المساحة: " + مساحة(ش))
نهاية`,exercise:'أنشئ نظام أوامر: صنف "أمر" (النوع، البيانات) واستخدم طابق لتنفيذ أوامر مختلفة (إضافة، حذف، تعديل، بحث).',hint:"تعداد نوع_الأمر مع 4 قيم، ثم طابق أمر.النوع وحسب القيمة نفذ العملية.",tips:["طابق خاصية الكائن وليس الكائن نفسه","التعدادات تسهل المطابقة — أرقام واضحة","نمط شائع لبناء أنظمة الأوامر"],concepts:["طابق مع كائنات","نظام أوامر"]},{id:"l3_38",title:"أنماط عملية للمطابقة",desc:"أنماط مطابقة شائعة في البرامج الحقيقية.",explanation:"آلة الحالة (State Machine) مثال مثالي: طلب يمر بحالات (جديد → مؤكد → شُحن → تم)، وكل حالة تستجيب للأحداث بشكل مختلف. معالجة استجابات API: طابق كود الحالة (200 نجاح، 404 غير موجود، 500 خطأ خادم). المطابقة تجعل هذه الأنماط واضحة وموثوقة.",code:`# نمط 1: معالجة استجابات API
دالة عالج_استجابة(الكود، البيانات)
    طابق (الكود)
        عندما 200:
            اطبع("✅ نجاح: " + البيانات)
        عندما 201:
            اطبع("✅ تم الإنشاء: " + البيانات)
        عندما 400:
            اطبع("❌ طلب خاطئ: " + البيانات)
        عندما 401:
            اطبع("🔒 غير مصرح")
        عندما 403:
            اطبع("🚫 ممنوع")
        عندما 404:
            اطبع("❓ غير موجود")
        عندما 500..600:
            اطبع("💥 خطأ خادم: " + الكود)
        افتراضي:
            اطبع("❔ كود غير معروف: " + الكود)
    نهاية
نهاية

عالج_استجابة(200، "قائمة المستخدمين")
عالج_استجابة(404، "/api/users/999")
عالج_استجابة(503، "الخادم مشغول")

# نمط 2: آلة حالة (State Machine)
دالة معالجة_طلب(الحالة، الحدث)
    طابق (الحالة)
        عندما "جديد":
            طابق (الحدث)
                عندما "تأكيد":
                    ارجع "مؤكد"
                عندما "إلغاء":
                    ارجع "ملغي"
                افتراضي:
                    ارجع "جديد"
            نهاية
        عندما "مؤكد":
            طابق (الحدث)
                عندما "شحن":
                    ارجع "شُحن"
                عندما "إلغاء":
                    ارجع "ملغي"
                افتراضي:
                    ارجع "مؤكد"
            نهاية
        عندما "شُحن":
            طابق (الحدث)
                عندما "تسليم":
                    ارجع "تم"
                افتراضي:
                    ارجع "شُحن"
            نهاية
        افتراضي:
            ارجع الحالة
    نهاية
نهاية

متغير حالة = "جديد"
اطبع("الحالة: " + حالة)

حالة = معالجة_طلب(حالة، "تأكيد")
اطبع("بعد التأكيد: " + حالة)

حالة = معالجة_طلب(حالة، "شحن")
اطبع("بعد الشحن: " + حالة)

حالة = معالجة_طلب(حالة، "تسليم")
اطبع("بعد التسليم: " + حالة)`,exercise:"أنشئ آلة حالة لمصعد: طوابق 0-5، أحداث (صعود، نزول، فتح، إغلاق) وطابق لتحديد السلوك.",hint:"طابق الحالة الحالية (الطابق) مع الحدث لتحديد الحالة الجديدة.",tips:["مطابقة الأنماط مثالية لآلات الحالة","طابق متداخل = فحص حالة + حدث","كل حالة تستجيب للأحداث بشكل مختلف"],concepts:["آلة حالة","معالجة أحداث","طابق متداخل"]},{id:"l3_39",title:"مشروع: محلل بيانات",desc:"مشروع يستخدم مطابقة الأنماط لتحليل ومعالجة البيانات.",explanation:"محلل البيانات يثبت قوة المطابقة: طابق نوع القيمة (رقم، نص، منطقي)، ثم داخلياً طابق التفاصيل (رقم صغير/كبير، نص قصير/طويل). المطابقة المتداخلة = تحليل متعدد المراحل. كود واضح يأخذ بيانات مختلفة ويعالجها بذكاء.",code:`# محلل بيانات باستخدام مطابقة الأنماط
دالة حلل_قيمة(القيمة)
    متغير النوع = نوع(القيمة)
    طابق (النوع)
        عندما "رقم":
            طابق (القيمة)
                عندما 0:
                    ارجع "صفر"
                عندما 1..100:
                    ارجع "رقم صغير"
                عندما 100..10000:
                    ارجع "رقم كبير"
                افتراضي:
                    ارجع "رقم ضخم"
            نهاية
        عندما "نص":
            إذا (طول(القيمة) == 0)
                ارجع "نص فارغ"
            وإلا
                إذا (طول(القيمة) < 10)
                    ارجع "نص قصير"
                وإلا
                    ارجع "نص طويل"
                نهاية
            نهاية
        عندما "منطقي":
            ارجع "قيمة منطقية: " + القيمة
        افتراضي:
            ارجع "نوع غير معروف"
    نهاية
نهاية

# اختبار المحلل
متغير بيانات = [0، 42، 5000، ""، "مرحبا"، "نص طويل جداً جداً"، صحيح، خطأ]
لكل ق في بيانات
    اطبع(ق + " → " + حلل_قيمة(ق))
نهاية

# تحليل بيانات منظمة
دالة حلل_سجل(سجل)
    طابق (سجل)
        عندما [الاسم، العمر، الدرجة]:
            اطبع("📊 " + الاسم + " | عمر: " + العمر + " | درجة: " + الدرجة)
            طابق (الدرجة)
                عندما 90..101:
                    اطبع("   ⭐ متفوق!")
                عندما 0..60:
                    اطبع("   ⚠️ يحتاج دعم")
                افتراضي:
                    اطبع("   ✅ جيد")
            نهاية
        افتراضي:
            اطبع("سجل غير صالح")
    نهاية
نهاية

حلل_سجل(["أحمد"، 20، 95])
حلل_سجل(["سارة"، 19، 72])
حلل_سجل(["خالد"، 21، 45])`,exercise:'أضف دالة "حلل_أمر" تأخذ مصفوفة [أمر، ...معاملات] وتنفذ: ["أضف"، اسم]، ["احذف"، رقم]، ["ابحث"، كلمة].',hint:'طابق المصفوفة: عندما ["أضف"، اسم]: ... عندما ["احذف"، رقم]: ...',tips:["مطابقة الأنماط ممتازة لتحليل البيانات","طابق متداخل = تحليل متعدد المراحل","نوع() تساعد في معرفة نوع القيمة"],concepts:["تحليل بيانات","طابق متداخل","معالجة"]}]},vc={id:"l3_adv_functions",title:"الدوال المتقدمة",icon:"🔧",lessons:[{id:"l3_40",title:"دوال لامدا",desc:"دوال قصيرة مجهولة تُكتب في سطر واحد.",explanation:'لامدا كورقة ملاحظة صغيرة بدلاً من كتاب كامل: "لامدا س: س * س" بدلاً من "دالة مربع(س) ارجع س * س نهاية". مثالية للعمليات البسيطة التي لا تحتاج اسماً. الصيغة: "لامدا معاملات: تعبير" — بدون "ارجع"، القيمة تُرجع تلقائياً. ستراها في كل مكان!',code:`# لامدا: دالة مختصرة بدون اسم
# الصيغة: لامدا معاملات: تعبير

متغير مربع = لامدا س: س * س
اطبع(مربع(5))     # 25
اطبع(مربع(10))    # 100

متغير جمع = لامدا أ، ب: أ + ب
اطبع(جمع(3، 7))   # 10

# لامدا بدون معاملات
متغير تحية = لامدا: "مرحباً بالعالم!"
اطبع(تحية())

# لامدا مع تعبير شرطي
متغير مطلق = لامدا ن: ن >= 0 ? ن : -ن
اطبع(مطلق(-5))    # 5
اطبع(مطلق(3))     # 3

# لامدا مع عمليات نصية
متغير صيغة = لامدا اسم: "مرحباً يا " + اسم + "!"
اطبع(صيغة("أحمد"))
اطبع(صيغة("سارة"))

# تخزين في مصفوفة
متغير عمليات = [
    لامدا أ، ب: أ + ب،
    لامدا أ، ب: أ - ب،
    لامدا أ، ب: أ * ب
]
اطبع(عمليات[0](10، 5))  # 15
اطبع(عمليات[1](10، 5))  # 5
اطبع(عمليات[2](10، 5))  # 50`,exercise:"أنشئ مصفوفة من 4 لامدا تمثل 4 عمليات حسابية، ثم اكتب حلقة تطبقها جميعاً على رقمين.",hint:"متغير عمليات = [لامدا أ، ب: أ + ب، لامدا أ، ب: أ - ب، ...]",tips:["لامدا = دالة مختصرة بسطر واحد","الصيغة: لامدا معاملات: تعبير","بدون أقواس حول المعاملات!"],concepts:["لامدا","دالة مجهولة","تعبير"]},{id:"l3_41",title:"دوال السهم",desc:"صيغة أخرى للدوال المختصرة باستخدام =>.",explanation:'دالة السهم صيغة بديلة: "(س) => س * 2" بدلاً من "لامدا س: س * 2". لماذا الخيار؟ السهم مألوف لمن يعرف JavaScript، ولامدا لمن يعرف Python. كلاهما يفعل نفس الشيء — اختر الأسهل قراءة لك. المهم: الأقواس ضرورية حول المعاملات.',code:`# دوال السهم: بديل آخر للامدا
# الصيغة: (معاملات) => تعبير

متغير ضعف = (س) => س * 2
اطبع(ضعف(5))     # 10

متغير جمع = (أ، ب) => أ + ب
اطبع(جمع(3، 4))  # 7

# بدون معاملات
متغير وقت = () => "الآن"
اطبع(وقت())

# الفرق بين لامدا والسهم:
# لامدا: لامدا س: س * 2
# السهم: (س) => س * 2
# كلاهما يعمل! اختر ما يناسبك

# مثال عملي: تحويلات
متغير سلسيوس_لفهرنهايت = (س) => س * 9 / 5 + 32
متغير كيلو_لأمتار = (ك) => ك * 1000
متغير ساعات_لدقائق = (س) => س * 60

اطبع("25°C = " + سلسيوس_لفهرنهايت(25) + "°F")
اطبع("5 كم = " + كيلو_لأمتار(5) + " متر")
اطبع("3 ساعات = " + ساعات_لدقائق(3) + " دقيقة")`,exercise:'أنشئ "آلة حاسبة وظيفية" — خريطة تربط اسم العملية بدالة سهم، ثم استخدمها.',hint:'متغير حاسبة = {"جمع": (أ، ب) => أ + ب, "طرح": (أ، ب) => أ - ب}، ثم حاسبة["جمع"](5, 3).',tips:["دوال السهم شائعة في JavaScript أيضاً","الأقواس ضرورية حول المعاملات","اختر لامدا أو السهم حسب تفضيلك"],concepts:["دالة سهم","=>","صيغة مختصرة"]},{id:"l3_42",title:"الدوال كقيم (First-class)",desc:"الدوال كمواطنين من الدرجة الأولى — تمريرها وإرجاعها.",explanation:'فكرة قوية: الدالة قيمة مثل الرقم والنص! يمكنك تخزينها في متغير، تمريرها كمعامل، وإرجاعها من دالة. تخيّل: دالة "مضاعف(3)" تُرجع دالة تضرب بـ 3! هذا المفهوم أساس البرمجة الوظيفية ويفتح أبواباً واسعة.',code:`# الدوال يمكن تخزينها وتمريرها مثل أي قيمة

# 1. تخزين في متغير
دالة تحية_رسمية(اسم)
    ارجع "السيد/ة " + اسم + " المحترم/ة"
نهاية

دالة تحية_ودية(اسم)
    ارجع "أهلاً يا " + اسم + "!"
نهاية

# تخزين الدالة في متغير (بدون أقواس = مرجع الدالة)
متغير طريقة_التحية = تحية_ودية
اطبع(طريقة_التحية("أحمد"))

طريقة_التحية = تحية_رسمية
اطبع(طريقة_التحية("أحمد"))

# 2. تمرير كمعامل
دالة طبّق(دالة_عملية، قيمة)
    ارجع دالة_عملية(قيمة)
نهاية

دالة ضعف(ن)
    ارجع ن * 2
نهاية

دالة تربيع(ن)
    ارجع ن * ن
نهاية

اطبع(طبّق(ضعف، 5))    # 10
اطبع(طبّق(تربيع، 5))   # 25

# 3. إرجاع دالة من دالة
دالة مضاعف(عامل)
    ارجع لامدا ن: ن * عامل
نهاية

متغير ثلاثة_أضعاف = مضاعف(3)
متغير خمسة_أضعاف = مضاعف(5)

اطبع(ثلاثة_أضعاف(10))  # 30
اطبع(خمسة_أضعاف(10))   # 50`,exercise:'اكتب دالة "سلسلة" تأخذ دالتين وتُرجع دالة جديدة تطبق الأولى ثم الثانية على الناتج.',hint:"دالة سلسلة(د1، د2) ارجع لامدا س: د2(د1(س)) نهاية.",tips:["First-class: الدوال = قيم مثل الأرقام والنصوص","يمكن تخزينها، تمريرها، إرجاعها","مفهوم قوي في البرمجة الوظيفية"],concepts:["دوال كقيم","تمرير دوال","إرجاع دوال"]},{id:"l3_43",title:"الإغلاقات (Closures)",desc:"الدوال التي تتذكر بيئتها — مفهوم الإغلاق.",explanation:'الإغلاق كحقيبة ظهر مع جيب سري: الدالة تحمل متغيرات من الخارج معها أينما ذهبت! "أنشئ_عداد()" تُرجع دالة تتذكر قيمتها بين الاستدعاءات — حالة خاصة بدون صنف! كل عداد منشأ مستقل — لا يتداخلون. مفهوم قوي جداً!',code:`# الإغلاق: دالة تتذكر المتغيرات من نطاقها الخارجي

# مثال 1: عداد
دالة أنشئ_عداد()
    متغير القيمة = 0

    دالة زيادة()
        القيمة += 1
        ارجع القيمة
    نهاية

    ارجع زيادة
نهاية

متغير عداد1 = أنشئ_عداد()
متغير عداد2 = أنشئ_عداد()

اطبع(عداد1())  # 1
اطبع(عداد1())  # 2
اطبع(عداد1())  # 3
اطبع(عداد2())  # 1 — عداد مستقل!

# مثال 2: منشئ رسائل
دالة منشئ_رسالة(البادئة)
    ارجع لامدا رسالة: البادئة + ": " + رسالة
نهاية

متغير خطأ = منشئ_رسالة("❌ خطأ")
متغير تحذير = منشئ_رسالة("⚠️ تحذير")
متغير معلومة = منشئ_رسالة("ℹ️ معلومة")

اطبع(خطأ("الملف غير موجود"))
اطبع(تحذير("الذاكرة منخفضة"))
اطبع(معلومة("تم التحديث"))

# مثال 3: ذاكرة تخزين مؤقت
دالة أنشئ_ذاكرة()
    متغير القيم = {}

    دالة احفظ(المفتاح، القيمة)
        القيم[المفتاح] = القيمة
    نهاية

    دالة اقرأ(المفتاح)
        إذا (القيم.يحتوي(المفتاح))
            ارجع القيم[المفتاح]
        نهاية
        ارجع لاشيء
    نهاية

    ارجع {"احفظ": احفظ، "اقرأ": اقرأ}
نهاية

متغير ذاكرة = أنشئ_ذاكرة()
ذاكرة["احفظ"]("اسم"، "أحمد")
اطبع(ذاكرة["اقرأ"]("اسم"))`,exercise:'أنشئ دالة "محدود" تأخذ دالة وحد أقصى لعدد الاستدعاءات، وتُرجع دالة تتوقف بعد الحد.',hint:"متغير عدد = 0 في الإغلاق، كل استدعاء يزيد عدد، إذا عدد > الحد لا تنفذ.",tips:["الإغلاق = دالة + بيئتها (المتغيرات المحيطة)","كل إغلاق يحتفظ بنسخته الخاصة من المتغيرات","مفيد لإنشاء حالة خاصة بدون أصناف"],concepts:["إغلاق","نطاق","حالة خاصة"]},{id:"l3_44",title:"المعاملات الافتراضية والأنواع",desc:"قيم افتراضية للمعاملات وتعليقات الأنواع.",explanation:'المعاملات الافتراضية كخيارات القهوة: "تحية(الاسم، اللقب = "السيد")" — إن لم تحدد اللقب، يُستخدم "السيد". تعليقات الأنواع (": رقم") للتوثيق والوضوح — اللغة لا تفرضها لكنها تساعد القارئ. هذان يجعلان الدوال أسهل استخداماً وفهماً.',code:`# معاملات افتراضية
دالة تحية(الاسم، اللقب = "السيد")
    اطبع("مرحباً " + اللقب + " " + الاسم)
نهاية

تحية("أحمد")                # مرحباً السيد أحمد
تحية("سارة"، "الدكتورة")    # مرحباً الدكتورة سارة

# عدة معاملات افتراضية
دالة إعداد_صفحة(العنوان، الحجم = 14، اللون = "أسود"، محاذاة = "يمين")
    اطبع("العنوان: " + العنوان)
    اطبع("الحجم: " + الحجم)
    اطبع("اللون: " + اللون)
    اطبع("المحاذاة: " + محاذاة)
نهاية

إعداد_صفحة("الصفحة الرئيسية")
اطبع("---")
إعداد_صفحة("مقال"، 16، "أزرق")

# تعليقات الأنواع (اختيارية)
دالة جمع(أ: رقم، ب: رقم): رقم
    ارجع أ + ب
نهاية

دالة تكرار(نص: نص، عدد: رقم): نص
    متغير نتيجة: نص = ""
    لكل ع في نطاق(0، عدد)
        نتيجة += نص
    نهاية
    ارجع نتيجة
نهاية

اطبع(جمع(5، 3))
اطبع(تكرار("ها"، 5))

# متغيرات مع أنواع
متغير العمر: رقم = 25
متغير الاسم: نص = "أحمد"
متغير نشط: منطقي = صحيح`,exercise:'اكتب دالة "أنشئ_مستخدم" بمعاملات: الاسم (مطلوب)، العمر = 18، البلد = "السعودية"، مفعّل = صحيح.',hint:'دالة أنشئ_مستخدم(الاسم، العمر = 18، البلد = "السعودية"، مفعّل = صحيح) ثم اطبع الخصائص.',tips:["المعاملات الافتراضية توضع في نهاية القائمة","تعليقات الأنواع توثيقية — لا تفرض نوعاً","الصيغة: معامل: نوع = قيمة_افتراضية"],concepts:["قيم افتراضية","تعليقات أنواع","توثيق"]},{id:"l3_45",title:"استيعاب القوائم",desc:"إنشاء مصفوفات بتعبيرات مختصرة — List Comprehension.",explanation:'بدلاً من حلقة من 4 أسطر، اكتب سطر واحد: "[س * س لكل س في نطاق(1، 11)]" يُنتج مربعات 1-10! يمكنك الفلترة: "[س لكل س ... إذا س > 0]" للموجبة فقط. كود أقصر، أوضح، وغالباً أسرع! متاح في أغلب اللغات الحديثة.',code:`# استيعاب القوائم: إنشاء مصفوفة بسطر واحد
# الصيغة: [تعبير لكل متغير في مصفوفة]

# مربعات الأرقام
متغير مربعات = [س * س لكل س في نطاق(1، 11)]
اطبع(مربعات)   # [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]

# مع فلترة (شرط إذا)
متغير زوجية = [س لكل س في نطاق(1، 21) إذا س % 2 == 0]
اطبع(زوجية)    # [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]

# تحويل نصوص
متغير أسماء = ["أحمد"، "سارة"، "خالد"]
متغير تحيات = ["مرحباً " + اسم لكل اسم في أسماء]
اطبع(تحيات)

# فلترة الأرقام الموجبة
متغير أرقام = [-5، 3، -1، 8، -2، 10، 0]
متغير موجبة = [ن لكل ن في أرقام إذا ن > 0]
اطبع(موجبة)    # [3, 8, 10]

# التحويل والفلترة معاً
متغير درجات = [45، 82، 91، 37، 68، 95، 55]
متغير ناجحون = [د لكل د في درجات إذا د >= 60]
اطبع("الناجحون: " + ناجحون)

# تعبيرات أكثر تعقيداً
متغير جدول_ضرب = [ق + " × 7 = " + (ق * 7) لكل ق في نطاق(1، 11)]
لكل سطر في جدول_ضرب
    اطبع(سطر)
نهاية`,exercise:"اكتب استيعاب قائمة يستخرج الكلمات التي طولها أكبر من 3 أحرف من مصفوفة نصوص.",hint:"متغير طويلة = [ك لكل ك في كلمات إذا طول(ك) > 3].",tips:["استيعاب القوائم أوضح وأسرع من الحلقات","الصيغة: [تعبير لكل متغير في مجموعة إذا شرط]","الشرط (إذا) اختياري"],concepts:["استيعاب القوائم","فلترة","تحويل"]},{id:"l3_46",title:"الدوال التكرارية المتقدمة",desc:"أنماط متقدمة في الاستدعاء الذاتي (Recursion).",explanation:"التكرار المتقدم: البحث الثنائي يقسم المصفوفة نصفين ويبحث في النصف الصحيح — سريع جداً! الأشجار طبيعياً تكرارية: كل فرع كشجرة صغيرة. التراكم (Accumulator) يحسّن الأداء — بدلاً من حفظ النتائج في الذاكرة، مرِّرها كمعامل. أنماط مهمة للخوارزميات.",code:`# التكرار المتقدم

# 1. تكرار مع تراكم (Accumulator)
دالة مضروب(ن، تراكم = 1)
    إذا (ن <= 1)
        ارجع تراكم
    نهاية
    ارجع مضروب(ن - 1، ن * تراكم)
نهاية

اطبع("5! = " + مضروب(5))    # 120

# 2. بحث ثنائي تكراري
دالة بحث_ثنائي(مصفوفة، هدف، بداية = 0، نهاية_ب = -1)
    إذا (نهاية_ب == -1)
        نهاية_ب = طول(مصفوفة) - 1
    نهاية

    إذا (بداية > نهاية_ب)
        ارجع -1
    نهاية

    متغير وسط = (بداية + نهاية_ب) / 2
    # تحويل لرقم صحيح
    وسط = إلى_رقم(وسط)

    إذا (مصفوفة[وسط] == هدف)
        ارجع وسط
    وإلا
        إذا (مصفوفة[وسط] > هدف)
            ارجع بحث_ثنائي(مصفوفة، هدف، بداية، وسط - 1)
        وإلا
            ارجع بحث_ثنائي(مصفوفة، هدف، وسط + 1، نهاية_ب)
        نهاية
    نهاية
نهاية

متغير أرقام = [2، 5، 8، 12، 16، 23، 38، 56، 72، 91]
اطبع("البحث عن 23: الموقع " + بحث_ثنائي(أرقام، 23))

# 3. شجرة عائلة (هيكل شجري)
دالة ارسم_شجرة(العمق، البادئة = "")
    إذا (العمق == 0)
        ارجع لاشيء
    نهاية
    اطبع(البادئة + "🌿 فرع (عمق " + العمق + ")")
    ارسم_شجرة(العمق - 1، البادئة + "  ")
    ارسم_شجرة(العمق - 1، البادئة + "  ")
نهاية

ارسم_شجرة(3)`,exercise:'اكتب دالة تكرارية "تسطيح" تأخذ مصفوفة متداخلة وتُرجع مصفوفة مسطحة.',hint:"إذا كان العنصر مصفوفة، استدعِ تسطيح عليه وأضف النتائج، وإلا أضف العنصر مباشرة.",tips:["التراكم (Accumulator) يحسّن أداء التكرار","البحث الثنائي: O(log n) — سريع جداً","الأشجار تُمثَّل طبيعياً بالتكرار"],concepts:["تكرار متقدم","بحث ثنائي","هياكل شجرية"]},{id:"l3_47",title:"مشروع: مكتبة دوال وظيفية",desc:"بناء مكتبة صغيرة من الدوال الوظيفية المفيدة.",explanation:'هذا المشروع يبني أدوات فعلية: "طبّق_على_الكل" (map) يطبق دالة على كل عنصر، "رشّح" (filter) يختار العناصر المطابقة، "اختزل" (reduce) يجمع الكل في قيمة واحدة. هذه الأنماط موجودة في كل لغة حديثة — تعلّمها يفتح أبواباً!',code:`# مكتبة دوال وظيفية

# 1. تطبيق دالة على كل عنصر
دالة طبّق_على_الكل(مصفوفة، دالة_عملية)
    متغير نتيجة = []
    لكل عنصر في مصفوفة
        نتيجة.أضف(دالة_عملية(عنصر))
    نهاية
    ارجع نتيجة
نهاية

# 2. فلترة
دالة رشّح(مصفوفة، دالة_شرط)
    متغير نتيجة = []
    لكل عنصر في مصفوفة
        إذا (دالة_شرط(عنصر))
            نتيجة.أضف(عنصر)
        نهاية
    نهاية
    ارجع نتيجة
نهاية

# 3. اختزال
دالة اختزل(مصفوفة، دالة_ع، قيمة_أولية)
    متغير تراكم = قيمة_أولية
    لكل عنصر في مصفوفة
        تراكم = دالة_ع(تراكم، عنصر)
    نهاية
    ارجع تراكم
نهاية

# 4. تركيب دالتين
دالة ركّب(د1، د2)
    ارجع لامدا س: د2(د1(س))
نهاية

# === الاستخدام ===
متغير أرقام = [1، 2، 3، 4، 5، 6، 7، 8، 9، 10]

# تطبيق: مربعات كل رقم
دالة ربّع(ن)
    ارجع ن * ن
نهاية
اطبع("المربعات: " + طبّق_على_الكل(أرقام، ربّع))

# فلترة: الأرقام الزوجية
دالة زوجي(ن)
    ارجع ن % 2 == 0
نهاية
اطبع("الزوجية: " + رشّح(أرقام، زوجي))

# اختزال: مجموع
دالة جمع2(أ، ب)
    ارجع أ + ب
نهاية
اطبع("المجموع: " + اختزل(أرقام، جمع2، 0))

# تركيب: ضعف ثم تربيع
دالة ضعف(ن)
    ارجع ن * 2
نهاية
متغير ضعف_ثم_ربّع = ركّب(ضعف، ربّع)
اطبع("ضعف 3 ثم تربيع: " + ضعف_ثم_ربّع(3))  # (3*2)^2 = 36`,exercise:'أضف دوال: "أي_عنصر" (ترجع صحيح إذا أي عنصر يحقق الشرط) و"كل_العناصر" (ترجع صحيح إذا كلها تحقق).',hint:"في أي_عنصر: إذا دالة_شرط(عنصر) ارجع صحيح. في كل_العناصر: إذا ليس دالة_شرط(عنصر) ارجع خطأ.",tips:["البرمجة الوظيفية = دوال نقية + تحويلات بيانات","هذه الأنماط موجودة في كل لغة حديثة","بناء مكتبة صغيرة = فهم عميق للدوال"],concepts:["برمجة وظيفية","تحويل","فلترة","اختزال"]}]},hc={id:"l3_modules",title:"الوحدات والاستيراد",icon:"📚",lessons:[{id:"l3_48",title:"مفهوم الوحدات",desc:"تقسيم الكود إلى ملفات — لماذا ولكيف.",explanation:"الوحدات كأدراج منظّمة: بدلاً من كومة ملابس واحدة، درج للقمصان، درج للبنطلونات. كل ملف وحدة مستقلة مسؤولة عن شيء واحد. الفوائد: تنظيم أفضل، إعادة استخدام (استورد ما تحتاج)، عمل جماعي (كل مطور يعمل على ملفه). أساس المشاريع الكبيرة!",code:`# الوحدات: تقسيم الكود إلى ملفات منظمة
# كل ملف .ص يمكن أن يكون وحدة

# ── لماذا الوحدات؟ ──
# 1. تنظيم الكود — كل ملف مسؤول عن مهمة
# 2. إعادة الاستخدام — استيراد ما تحتاجه
# 3. تجنب التكرار — كتابة الكود مرة واحدة
# 4. العمل الجماعي — كل مطور يعمل على ملفه

# ── مثال: ملف رياضيات.ص ──
# (تصوّر أن هذا ملف منفصل)
# صدّر دالة جمع(أ، ب)
#     ارجع أ + ب
# نهاية
#
# صدّر دالة ضرب(أ، ب)
#     ارجع أ * ب
# نهاية

# ── في الملف الرئيسي ──
# استورد "رياضيات"
# اطبع(جمع(5، 3))

# مثال يعمل بدون ملفات خارجية:
# نمذجة المفهوم
دالة محاكاة_وحدة()
    اطبع("📦 تخيل هذا ملفاً منفصلاً!")
    اطبع("يحتوي على دوال وثوابت")
    اطبع("يمكن استيراده من أي ملف آخر")
نهاية

محاكاة_وحدة()

# بنية المشروع المثالية:
# مشروع/
#   ├── رئيسي.ص       ← نقطة الدخول
#   ├── رياضيات.ص     ← دوال حسابية
#   ├── نصوص.ص        ← معالجة نصوص
#   └── مساعدات.ص     ← دوال مساعدة

اطبع("\\n📁 بنية المشروع المنظمة:")
اطبع("  رئيسي.ص — نقطة الدخول")
اطبع("  رياضيات.ص — الحسابات")
اطبع("  أدوات.ص — دوال مساعدة")`,exercise:"ارسم بنية مشروع (كنصوص مطبوعة) يتكون من 5+ ملفات لنظام إدارة مكتبة.",hint:"مثلاً: كتب.ص، أعضاء.ص، إعارة.ص، بحث.ص، واجهة.ص، رئيسي.ص.",tips:["كل ملف .ص وحدة مستقلة","صدّر لمشاركة الدوال مع ملفات أخرى","استورد لاستخدام دوال من ملف آخر"],concepts:["وحدة","تنظيم","تقسيم الكود"]},{id:"l3_49",title:"التصدير (صدّر)",desc:"كيفية تصدير الدوال والمتغيرات من ملف.",explanation:'"صدّر" كوضع البضاعة في واجهة المحل: ما تصدّره يراه العالم، ما لا تصدّره يبقى في المخزن. "صدّر دالة جمع()" يتيح للملفات الأخرى استخدامها، دالة بدون "صدّر" خاصة بالملف. القاعدة: صدّر فقط ما يحتاجه المستخدمون، أخفِ التفاصيل.',code:`# صدّر: جعل الدالة أو المتغير متاحاً للاستيراد
# يُستخدم قبل التعريف

# تصدير دالة
صدّر دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

# تصدير متغير ثابت
صدّر ثابت النسخة = "1.0.0"

# تصدير صنف
صدّر صنف نقطة
    باني(س، ص)
        هذا.س = س
        هذا.ص = ص
    نهاية

    دالة المسافة(أخرى)
        متغير فس = هذا.س - أخرى.س
        متغير فص = هذا.ص - أخرى.ص
        ارجع (فس * فس + فص * فص) ** 0.5
    نهاية
نهاية

# دالة غير مصدرة — خاصة بالملف فقط
دالة مساعدة_داخلية()
    ارجع "لا يمكن الوصول لي من الخارج"
نهاية

# ما لا يُصدَّر لا يُرى من الخارج
# هذا يعني أن الملف يتحكم في ما يشاركه

# تجربة
اطبع(جمع(10، 20))
اطبع("النسخة: " + النسخة)

متغير ن1 = جديد نقطة(0، 0)
متغير ن2 = جديد نقطة(3، 4)
اطبع("المسافة: " + ن1.المسافة(ن2))`,exercise:'اكتب وحدة "محول_وحدات" تصدّر 5 دوال تحويل: كيلو←مايل، سلسيوس←فهرنهايت، كغ←رطل، متر←قدم، لتر←غالون.',hint:"صدّر دالة كيلو_لمايل(كم) ارجع كم * 0.621 نهاية وهكذا لباقي الدوال.",tips:['"صدّر" تُوضع قبل دالة أو متغير أو ثابت أو صنف',"ما لا يُصدَّر يبقى خاصاً بالملف","صدّر فقط ما يحتاجه المستخدمون"],concepts:["صدّر","واجهة عامة","إخفاء التفاصيل"]},{id:"l3_50",title:"الاستيراد (استورد)",desc:"كيفية استيراد واستخدام دوال من ملفات أخرى.",explanation:'ثلاثة أساليب: "استورد "ملف"" يجلب الكل، "استورد { جمع } من "ملف"" يجلب المحدد فقط، "استورد ... كـ ر" يعطي اسماً مختصراً. الاستيراد الانتقائي أفضل للأداء — لا تحمّل ما لا تحتاج. المسار نسبي بدون امتداد .ص.',code:`# استورد: جلب دوال ومتغيرات من ملف آخر
# الصيغة: استورد "اسم_الملف"

# ── الاستيراد الكامل ──
# استورد "رياضيات"
# هذا يجلب كل ما صُدِّر من رياضيات.ص

# ── الاستيراد الانتقائي ──
# استورد { جمع، ضرب } من "رياضيات"
# هذا يجلب فقط جمع وضرب

# ── الاستيراد بلقب ──
# استورد "رياضيات" كـ ر
# ر.جمع(5، 3)

# مثال تعليمي (محاكاة بدون ملفات فعلية):
اطبع("=== طرق الاستيراد ===")
اطبع("1️⃣ استورد \\"رياضيات\\"")
اطبع("   → يجلب كل الدوال المصدرة")
اطبع("")
اطبع("2️⃣ استورد { جمع، ضرب } من \\"رياضيات\\"")
اطبع("   → يجلب دوال محددة فقط")
اطبع("")
اطبع("3️⃣ استورد \\"رياضيات\\" كـ ر")
اطبع("   → يجلب كل شيء تحت اسم مختصر")
اطبع("   → الاستخدام: ر.جمع(5، 3)")

# مثال عملي: بنية ملفات حقيقية
اطبع("\\n=== مثال عملي ===")
اطبع("# ملف: أدوات.ص")
اطبع("صدّر دالة سجل(رسالة)")
اطبع("    اطبع(\\"[سجل] \\" + رسالة)")
اطبع("نهاية")
اطبع("")
اطبع("# ملف: رئيسي.ص")
اطبع("استورد \\"أدوات\\"")
اطبع("سجل(\\"بدأ التشغيل\\")")`,exercise:"اكتب (كنصوص مطبوعة) ملفين: utils.ص يصدّر 3 دوال، وmain.ص يستورد ويستخدمها.",hint:"ملف 1 يصدّر دوال، ملف 2 يستورد ويستدعيها. اطبع الشفرة كنصوص.",tips:['استورد "ملف" بدون امتداد .ص','المسار نسبي — "./مجلد/ملف" أو "ملف" فقط',"الاستيراد الانتقائي أفضل للأداء"],concepts:["استورد","من","كـ","استيراد انتقائي"]},{id:"l3_51",title:"تنظيم المشاريع",desc:"أفضل الممارسات لتنظيم مشروع كبير بلغة ص.",explanation:'البنية النموذجية: رئيسي.ص (نقطة الدخول)، مجلد "نماذج/" للأصناف، "خدمات/" للمنطق، "أدوات/" للمساعدين. الملف الرئيسي ينسق فقط — لا منطق ثقيل. كل ملف مسؤول عن شيء واحد. هذا التنظيم يسهّل الصيانة والتوسيع.',code:`# تنظيم المشروع — أفضل الممارسات

# بنية مشروع نموذجية:
اطبع("📁 مشروع_متجر/")
اطبع("├── رئيسي.ص            ← نقطة الدخول")
اطبع("├── إعدادات.ص          ← الثوابت والإعدادات")
اطبع("├── نماذج/")
اطبع("│   ├── منتج.ص         ← صنف المنتج")
اطبع("│   ├── مستخدم.ص       ← صنف المستخدم")
اطبع("│   └── طلب.ص          ← صنف الطلب")
اطبع("├── خدمات/")
اطبع("│   ├── مصادقة.ص       ← تسجيل الدخول")
اطبع("│   ├── سلة.ص          ← إدارة السلة")
اطبع("│   └── دفع.ص          ← معالجة الدفع")
اطبع("├── أدوات/")
اطبع("│   ├── سجل.ص          ← التسجيل")
اطبع("│   ├── تحقق.ص         ← التحقق من البيانات")
اطبع("│   └── تنسيق.ص        ← تنسيق العرض")
اطبع("└── اختبارات/")
اطبع("    ├── اختبار_منتج.ص")
اطبع("    └── اختبار_سلة.ص")

# مبادئ التنظيم:
اطبع("\\n🎯 مبادئ التنظيم:")
اطبع("1. كل ملف مسؤول عن شيء واحد")
اطبع("2. المجلدات تجمع الملفات المترابطة")
اطبع("3. صدّر فقط الواجهة العامة")
اطبع("4. الملف الرئيسي ينسق فقط — لا منطق ثقيل")

# تطبيق عملي: ملف إعدادات
# عادة يكون ملف إعداداات.ص هكذا:
ثابت اسم_التطبيق = "متجر لغة ص"
ثابت النسخة = "1.0.0"
ثابت الحد_الأقصى_للسلة = 50

# تطبيق عملي: ملف أدوات بسيط
دالة سجّل(المستوى، الرسالة)
    اطبع("[" + المستوى + "] " + الرسالة)
نهاية

دالة تحقق_بريد(البريد)
    ارجع البريد.يحتوي("@") و البريد.يحتوي(".")
نهاية

سجّل("معلومة"، اسم_التطبيق + " نسخة " + النسخة)
اطبع("بريد صالح؟ " + تحقق_بريد("test@email.com"))`,exercise:'صمم بنية مجلدات لمشروع "شبكة اجتماعية" مع 10+ ملفات على الأقل.',hint:"فكر في: مستخدمين، منشورات، تعليقات، إعجابات، رسائل، إشعارات، إعدادات.",tips:["ملف واحد = مسؤولية واحدة","استخدم مجلدات لتنظيم الموضوعات","الملف الرئيسي = المنسق فقط"],concepts:["هيكل المشروع","مسؤولية واحدة","تنظيم"]},{id:"l3_52",title:"أنماط الاستيراد والتصدير",desc:"أنماط شائعة في تنظيم الوحدات والمكتبات.",explanation:"أنماط شائعة: 1) مكتبة دوال — مجموعة دوال مساعدة. 2) صنف + دوال مساعدة — كصنف مستخدم مع دالة إنشاء. 3) ثوابت مشتركة — إعدادات وحدود. المكتبة الجيدة تصدّر واجهة بسيطة وتخفي التفاصيل الداخلية.",code:`# أنماط شائعة للوحدات

# نمط 1: مكتبة دوال مساعدة
# ملف: مساعدات.ص
# صدّر دالة تحقق_فارغ(نص)
#     ارجع طول(نص.قص()) == 0
# نهاية
# صدّر دالة أول_حرف_كبير(نص)
#     ارجع نص.جزء(0، 1).لأكبر() + نص.جزء(1)
# نهاية

# نمط 2: صنف + دوال مساعدة
# ملف: مستخدم.ص
# صدّر صنف مستخدم
#     باني(الاسم، البريد)
#         هذا.الاسم = الاسم
#         هذا.البريد = البريد
#     نهاية
# نهاية
# صدّر دالة أنشئ_مستخدم(الاسم، البريد)
#     ارجع جديد مستخدم(الاسم، البريد)
# نهاية

# نمط 3: ثوابت مشتركة
# ملف: ثوابت.ص
# صدّر ثابت حد_كلمة_السر = 8
# صدّر ثابت حد_الاسم = 50
# صدّر ثابت ألوان = {
#     "نجاح": "أخضر"،
#     "خطأ": "أحمر"،
#     "تحذير": "أصفر"
# }

# تطبيق عملي يعمل:
صنف سجل_أحداث
    باني(الاسم)
        هذا.الاسم = الاسم
        هذا.الأحداث = []
    نهاية

    دالة أضف(حدث)
        هذا.الأحداث.أضف({"حدث": حدث})
        اطبع("[" + هذا.الاسم + "] " + حدث)
    نهاية

    دالة عرض()
        اطبع("📋 سجل " + هذا.الاسم + ":")
        لكل ح في هذا.الأحداث
            اطبع("  • " + ح["حدث"])
        نهاية
    نهاية
نهاية

# أنشئ سجلاً كمكتبة جاهزة
متغير سجل = جديد سجل_أحداث("النظام")
سجل.أضف("بدأ التشغيل")
سجل.أضف("تم تحميل الإعدادات")
سجل.أضف("جاهز للعمل")
سجل.عرض()`,exercise:'اكتب مكتبة "تحقق" تحتوي على 5 دوال تحقق: بريد، رقم هاتف، عمر، اسم، كلمة سر.',hint:"دالة تحقق_بريد(ب): يحتوي @ و. | دالة تحقق_هاتف(ه): طول == 10 | إلخ.",tips:["المكتبة = مجموعة دوال/أصناف متعلقة","صدّر فقط الواجهة — أخفِ التفاصيل","اسم الملف = اسم المكتبة"],concepts:["مكتبة","أنماط وحدات","واجهة عامة"]},{id:"l3_53",title:"مشروع: مكتبة متكاملة",desc:"بناء مكتبة كاملة قابلة للاستيراد.",explanation:'هذا المشروع يبني مكتبة "أدوات_نصوص" متكاملة: قسم التحقق (بريد صالح؟ فارغ؟)، قسم التنسيق (بادئة أصفار، اقتباس)، قسم التحويل (كلمات، عدّ). المكتبة الجيدة: واضحة، موثقة، مقسّمة حسب الوظيفة، وأسماؤها متسقة.',code:`# مشروع: مكتبة "أدوات_نصوص" المتكاملة
# تخيل أن كل قسم ملف منفصل

# === القسم 1: التحقق ===
دالة فارغ(نص)
    ارجع طول(نص) == 0
نهاية

دالة بريد_صالح(بريد)
    ارجع بريد.يحتوي("@") و بريد.يحتوي(".")
نهاية

دالة رقم_فقط(نص)
    لكل ح في نص.قسم("")
        إذا (ليس "0123456789".يحتوي(ح))
            ارجع خطأ
        نهاية
    نهاية
    ارجع صحيح
نهاية

# === القسم 2: التنسيق ===
دالة بادئة_أصفار(رقم، طول_مطلوب)
    متغير نص = إلى_نص(رقم)
    بينما (طول(نص) < طول_مطلوب)
        نص = "0" + نص
    نهاية
    ارجع نص
نهاية

دالة اقتبس(نص)
    ارجع "\\"" + نص + "\\""
نهاية

دالة كرر_حرف(حرف، عدد)
    متغير نتيجة = ""
    لكل ع في نطاق(0، عدد)
        نتيجة += حرف
    نهاية
    ارجع نتيجة
نهاية

# === القسم 3: التحويل ===
دالة كلمات(نص)
    ارجع نص.قسم(" ")
نهاية

دالة عدد_الكلمات(نص)
    ارجع طول(كلمات(نص))
نهاية

# === الاستخدام ===
اطبع("=== اختبار المكتبة ===")

اطبع("فارغ(''): " + فارغ(""))
اطبع("فارغ('مرحبا'): " + فارغ("مرحبا"))

اطبع("بريد صالح: " + بريد_صالح("test@email.com"))
اطبع("بريد خاطئ: " + بريد_صالح("invalid"))

اطبع("رقم فقط '123': " + رقم_فقط("123"))
اطبع("رقم فقط 'abc': " + رقم_فقط("abc"))

اطبع("بادئة أصفار: " + بادئة_أصفار(42، 5))
اطبع("اقتبس: " + اقتبس("مرحبا"))
اطبع("خط: " + كرر_حرف("═"، 20))

اطبع("عدد الكلمات: " + عدد_الكلمات("مرحبا بك في لغة ص"))`,exercise:'أضف قسم "البحث" للمكتبة: دالة يبدأ_بـ()، ينتهي_بـ()، عدد_التكرار(نص، حرف).',hint:"دالة عدد_التكرار: حلقة على كل حرف مع عداد لكل تطابق.",tips:["المكتبة الجيدة: واضحة، موثقة، مختبرة","قسّم المكتبة حسب الوظيفة","وفر أسماء عربية واضحة ومتسقة"],concepts:["مكتبة كاملة","تنظيم أقسام","اختبار"]}]},fc={id:"l3_async",title:"البرمجة غير المتزامنة",icon:"⚡",lessons:[{id:"l3_54",title:"مفهوم البرمجة غير المتزامنة",desc:"ما هي البرمجة غير المتزامنة ولماذا نحتاجها.",explanation:'تخيّل طباخاً ينتظر الماء يغلي واقفاً — ضياع وقت! الطباخ الذكي يقطع الخضار أثناء غليان الماء. البرمجة غير المتزامنة تفعل نفس الشيء: أثناء انتظار الشبكة، نفّذ مهام أخرى. "غير_متزامن" و"انتظر" هما المفتاح. أساسي لتطبيقات الويب الحديثة!',code:`# البرمجة غير المتزامنة: تنفيذ مهام دون انتظار
# تخيل: طلب بيانات من الإنترنت يستغرق وقتاً
# بدلاً من الانتظار → نفذ مهام أخرى!

# مقارنة:
# التزامني (العادي): اطبخ → انتظر → قدّم → اطبخ التالي
# غير متزامن: اطبخ الكل → عندما ينضج أي طبق → قدّمه

# مثال بسيط
غير_متزامن دالة جلب_بيانات(المصدر)
    اطبع("⏳ جاري جلب البيانات من " + المصدر + "...")
    # في الواقع هنا سيكون طلب شبكة
    ارجع "بيانات من " + المصدر
نهاية

غير_متزامن دالة معالجة()
    اطبع("🚀 بدء المعالجة")

    # "انتظر" تنتظر حتى تكتمل العملية
    متغير نتيجة = انتظر جلب_بيانات("قاعدة البيانات")
    اطبع("✅ " + نتيجة)

    متغير نتيجة2 = انتظر جلب_بيانات("الخادم")
    اطبع("✅ " + نتيجة2)

    اطبع("🏁 اكتملت المعالجة")
نهاية

معالجة()`,exercise:"اكتب دالة غير متزامنة تحاكي تحميل 3 صور (اطبع رسالة لكل صورة عند اكتمال التحميل).",hint:'غير_متزامن دالة حمّل_صورة(اسم) ... ثم انتظر حمّل_صورة("صورة1") لكل صورة.',tips:['"غير_متزامن" تُوضع قبل "دالة"','"انتظر" تنتظر اكتمال العملية',"مفهوم أساسي في التطبيقات الحديثة"],concepts:["غير_متزامن","انتظر","تزامن"]},{id:"l3_55",title:"الدوال غير المتزامنة",desc:"كتابة واستخدام الدوال غير المتزامنة.",explanation:'"غير_متزامن دالة" تُرجع وعداً (Promise): "سأعطيك النتيجة لاحقاً". "انتظر" يريح البرنامج حتى يجهز الوعد. الفائدة: الكود يبدو عادياً (سطرياً) رغم أنه غير متزامن! بدون "callbacks" معقدة. العمليات المتتالية بـ "انتظر" تنفَّذ بالتسلسل الصحيح.',code:`# كل دالة غير متزامنة تُرجع وعداً (Promise)
# "انتظر" يستخرج القيمة من الوعد

غير_متزامن دالة حساب_معقد(ن)
    # محاكاة عملية طويلة
    متغير نتيجة = 0
    لكل ع في نطاق(1، ن + 1)
        نتيجة += ع
    نهاية
    ارجع نتيجة
نهاية

غير_متزامن دالة جلب_مستخدم(المعرف)
    # محاكاة طلب API
    ارجع {"المعرف": المعرف، "الاسم": "مستخدم_" + المعرف}
نهاية

# استخدام
غير_متزامن دالة رئيسية()
    # العمليات الحسابية
    متغير مجموع = انتظر حساب_معقد(100)
    اطبع("مجموع 1..100 = " + مجموع)

    # جلب بيانات
    متغير مستخدم = انتظر جلب_مستخدم(42)
    اطبع("المستخدم: " + مستخدم["الاسم"])

    # دوال غير متزامنة متتالية
    متغير م1 = انتظر جلب_مستخدم(1)
    متغير م2 = انتظر جلب_مستخدم(2)
    متغير م3 = انتظر جلب_مستخدم(3)

    اطبع("المستخدمون:")
    اطبع("  " + م1["الاسم"])
    اطبع("  " + م2["الاسم"])
    اطبع("  " + م3["الاسم"])
نهاية

رئيسية()`,exercise:'اكتب دالة غير متزامنة "معالج_الطلبات" تجلب بيانات من 3 مصادر وتدمجها.',hint:"انتظر كل مصدر ثم ادمج النتائج في خريطة أو مصفوفة واحدة.",tips:["الدوال غير المتزامنة تُرجع وعوداً تلقائياً",'"انتظر" يعمل فقط داخل دالة غير متزامنة','العمليات المتتالية تنفذ بالترتيب مع "انتظر"'],concepts:["وعد","دالة غير متزامنة","تتابع"]},{id:"l3_56",title:"معالجة الأخطاء غير المتزامنة",desc:"التعامل مع الأخطاء في العمليات غير المتزامنة.",explanation:'الشبكة قد تفشل! "حاول/امسك" يعمل تماماً مع "انتظر": "حاول { متغير ن = انتظر جلب() } امسك (خ) { ... }". نمط إعادة المحاولة: إذا فشل، حاول مرة/مرات أخرى. "أخيراً" يُنفّذ دائماً — للتنظيف. التعامل الجيد مع الأخطاء يجعل التطبيق موثوقاً.',code:`# الأخطاء غير المتزامنة تُمسك بـ حاول/امسك

غير_متزامن دالة جلب_بيانات(العنوان)
    إذا (العنوان == "خطأ")
        ارمي "فشل الاتصال بـ " + العنوان
    نهاية
    ارجع "بيانات من " + العنوان
نهاية

# معالجة الأخطاء
غير_متزامن دالة جلب_آمن(العنوان)
    حاول
        متغير نتيجة = انتظر جلب_بيانات(العنوان)
        اطبع("✅ " + نتيجة)
        ارجع نتيجة
    امسك (خطأ)
        اطبع("❌ خطأ: " + خطأ)
        ارجع لاشيء
    أخيراً
        اطبع("🏁 انتهت المحاولة لـ " + العنوان)
    نهاية
نهاية

# اختبار
غير_متزامن دالة اختبار()
    انتظر جلب_آمن("الخادم")
    اطبع("---")
    انتظر جلب_آمن("خطأ")
    اطبع("---")
    انتظر جلب_آمن("قاعدة البيانات")
نهاية

اختبار()

# نمط: إعادة المحاولة
غير_متزامن دالة جلب_مع_إعادة(العنوان، محاولات = 3)
    لكل م في نطاق(1، محاولات + 1)
        حاول
            متغير نتيجة = انتظر جلب_بيانات(العنوان)
            اطبع("✅ نجح في المحاولة " + م)
            ارجع نتيجة
        امسك (خ)
            اطبع("⚠️ محاولة " + م + " فشلت: " + خ)
            إذا (م == محاولات)
                اطبع("❌ فشلت جميع المحاولات!")
                ارجع لاشيء
            نهاية
        نهاية
    نهاية
نهاية`,exercise:'اكتب دالة "جلب_مع_بديل" تحاول جلب البيانات، وإذا فشلت تحاول من مصدر بديل.',hint:"حاول المصدر الأول، إذا فشل (امسك) حاول المصدر البديل.",tips:["حاول/امسك يعمل مع الدوال غير المتزامنة",'"أخيراً" يُنفذ دائماً — نجاح أو فشل',"إعادة المحاولة نمط شائع في الشبكات"],concepts:["أخطاء غير متزامنة","إعادة المحاولة"]},{id:"l3_57",title:"أنماط غير متزامنة شائعة",desc:"أنماط عملية للبرمجة غير المتزامنة.",explanation:"أنماط مهمة: 1) التتالي — كل خطوة تعتمد على السابقة (جلب مستخدم → جلب طلباته). 2) التخزين المؤقت — احفظ النتيجة لتجنب إعادة الجلب. 3) التحميل عند الحاجة (Lazy) — لا تجلب حتى تُطلَب. هذه الأنماط تجعل التطبيقات سريعة وفعّالة.",code:`# نمط 1: تحميل بيانات متتالية (كل خطوة تعتمد على السابقة)
غير_متزامن دالة جلب_ملف_شخصي(المعرف)
    ارجع {"الاسم": "أحمد"، "المعرف": المعرف}
نهاية

غير_متزامن دالة جلب_طلبات(المعرف)
    ارجع [{"رقم": 1، "مبلغ": 100}، {"رقم": 2، "مبلغ": 200}]
نهاية

غير_متزامن دالة عرض_لوحة(معرف_المستخدم)
    # خطوة 1: جلب المستخدم
    متغير مستخدم = انتظر جلب_ملف_شخصي(معرف_المستخدم)
    اطبع("مرحباً " + مستخدم["الاسم"])

    # خطوة 2: جلب طلباته (تعتمد على المستخدم)
    متغير طلبات = انتظر جلب_طلبات(مستخدم["المعرف"])
    اطبع("عدد الطلبات: " + طول(طلبات))

    لكل ط في طلبات
        اطبع("  طلب #" + ط["رقم"] + " — " + ط["مبلغ"] + " ريال")
    نهاية
نهاية

انتظر عرض_لوحة(42)

# نمط 2: تحميل وتخزين مؤقت
صنف مخزن_مؤقت
    باني()
        هذا.البيانات = {}
    نهاية

    غير_متزامن دالة جلب(المفتاح)
        إذا (هذا.البيانات.يحتوي(المفتاح))
            اطبع("📦 من الذاكرة: " + المفتاح)
            ارجع هذا.البيانات[المفتاح]
        نهاية

        اطبع("🌐 جلب: " + المفتاح)
        متغير قيمة = "بيانات_" + المفتاح
        هذا.البيانات[المفتاح] = قيمة
        ارجع قيمة
    نهاية
نهاية

متغير مخزن = جديد مخزن_مؤقت()
انتظر مخزن.جلب("مستخدم_1")   # 🌐 جلب
انتظر مخزن.جلب("مستخدم_1")   # 📦 من الذاكرة
انتظر مخزن.جلب("مستخدم_2")   # 🌐 جلب`,exercise:'أنشئ صنف "مدير_تحميل" يحمّل ملفات بالترتيب مع شريط تقدم (نسبة مئوية مطبوعة).',hint:"صنف مع دالة غير متزامنة حمّل(ملفات) تستخدم حلقة مع عداد للتقدم.",tips:["التتالي: كل خطوة تنتظر السابقة","التخزين المؤقت: تجنب الجلب المتكرر","غير_متزامن يعمل مع الأصناف أيضاً"],concepts:["تتالي","تخزين مؤقت","أنماط"]},{id:"l3_58",title:"معالجة F-strings والنصوص المنسقة",desc:"استخدام النصوص المنسقة (F-strings) لبناء نصوص ديناميكية.",explanation:'f-strings أسهل من الدمج بـ +: "ف"مرحباً {الاسم}!"" بدلاً من ""مرحباً " + الاسم + "!"". داخل {} أي تعبير صالح: {عمر + 5}، {كائن.اسم()}. أوضح وأقل أخطاء (نسيان مسافة). مثالية للفواتير والرسائل والتقارير.',code:`# F-strings: نصوص منسقة بتعبيرات داخلية
# الصيغة: f"نص {تعبير} نص"

متغير الاسم = "أحمد"
متغير العمر = 25

# أساسي
اطبع(f"مرحباً يا {الاسم}!")
اطبع(f"عمرك {العمر} سنة")

# تعبيرات حسابية
اطبع(f"بعد 5 سنوات ستكون {العمر + 5}")
اطبع(f"3 × 7 = {3 * 7}")

# مع متغيرات متعددة
متغير المدينة = "الرياض"
متغير البلد = "السعودية"
اطبع(f"{الاسم} من {المدينة}، {البلد}")

# مع دوال
دالة تحية(اسم)
    ارجع f"أهلاً وسهلاً يا {اسم}!"
نهاية
اطبع(تحية("سارة"))

# مع خصائص الكائنات
صنف منتج
    باني(الاسم، السعر)
        هذا.الاسم = الاسم
        هذا.السعر = السعر
    نهاية

    دالة فاتورة()
        ارجع f"المنتج: {هذا.الاسم} — السعر: {هذا.السعر} ريال"
    نهاية
نهاية

متغير م = جديد منتج("قهوة"، 25)
اطبع(م.فاتورة())

# بناء جداول
اطبع(f"{'الاسم':-<15} {'العمر':-<10} {'المدينة':-<15}")
متغير أشخاص = [
    ["أحمد"، 25، "الرياض"]،
    ["سارة"، 30، "جدة"]،
    ["خالد"، 28، "الدمام"]
]
لكل ش في أشخاص
    اطبع(f"{ش[0]:-<15} {ش[1]:-<10} {ش[2]:-<15}")
نهاية`,exercise:'اكتب دالة "فاتورة" تأخذ مصفوفة منتجات وتطبع فاتورة منسقة باستخدام f-strings.',hint:'استخدم حلقة وf"" لكل سطر في الفاتورة مع المجموع في النهاية.',tips:['f"" تبدأ حرف f صغير (لاتيني)',"التعبيرات داخل {} يمكن أن تكون أي كود صالح","أوضح وأقصر من + للدمج"],concepts:["f-string","تنسيق نصوص","تعبيرات داخلية"]},{id:"l3_59",title:"التأشير على الأنواع",desc:"إضافة تعليقات الأنواع لتوثيق الكود وتحسين قراءته.",explanation:'تعليقات الأنواع كلافتات على العلب: "دالة جمع(أ: رقم، ب: رقم): رقم" توضح فوراً ماذا تتوقع وتُرجع. لغة ص لا تفرضها، لكنها تساعد القارئ والمحرر (لإكمال الكود). الأنواع: رقم، عشري، نص، منطقي، فراغ. عادة جيدة للتوثيق!',code:`# تعليقات الأنواع: توثيق يوضح نوع البيانات المتوقع
# لغة ص لا تفرض الأنواع — لكن التعليقات تساعد

# على المتغيرات
متغير العمر: رقم = 25
متغير الاسم: نص = "أحمد"
متغير نشط: منطقي = صحيح
متغير النسبة: عشري = 3.14

# على الدوال
دالة جمع(أ: رقم، ب: رقم): رقم
    ارجع أ + ب
نهاية

دالة تحية(الاسم: نص): نص
    ارجع "مرحباً " + الاسم
نهاية

دالة تحقق(القيمة: رقم): منطقي
    ارجع القيمة > 0
نهاية

# دالة بدون إرجاع
دالة سجّل(رسالة: نص): فراغ
    اطبع("[سجل] " + رسالة)
نهاية

# الاستخدام
اطبع(جمع(5، 3))
اطبع(تحية("سارة"))
اطبع(تحقق(42))
سجّل("بدأ البرنامج")

# مع الأصناف
صنف حساب
    باني(الاسم: نص، الرصيد: رقم)
        هذا.الاسم = الاسم
        هذا.الرصيد = الرصيد
    نهاية

    دالة إيداع(مبلغ: رقم): فراغ
        هذا.الرصيد += مبلغ
    نهاية

    دالة الرصيد_الحالي(): رقم
        ارجع هذا.الرصيد
    نهاية
نهاية

متغير حسابي: حساب = جديد حساب("أحمد"، 1000)
حسابي.إيداع(500)
اطبع(f"الرصيد: {حسابي.الرصيد_الحالي()}")`,exercise:"أعد كتابة أي 3 دوال من دروس سابقة مع إضافة تعليقات أنواع كاملة.",hint:"أضف : نوع بعد كل معامل وبعد () لنوع الإرجاع.",tips:["تعليقات الأنواع = توثيق مفيد","الأنواع: رقم، عشري، نص، منطقي، فراغ","لا تفرض نوعاً — لكنها تحسن القراءة"],concepts:["تأشير أنواع","توثيق","أنواع البيانات"]},{id:"l3_60",title:"المشروع الختامي: نظام إدارة مشاريع",desc:"مشروع شامل يجمع جميع مواضيع المستوى الثالث.",explanation:'🎓 تتويج المستوى الثالث! هذا المشروع يجمع كل شيء: أصناف + وراثة + بنيات + تعدادات + طابق + لامدا + f-strings + تعليقات أنواع. "مشروع" فيه "مهام"، المهمة لها حالة (تعداد) وأولوية، والمشروع يحسب التقدم. الآن أنت مبرمج كائني بلغة ص!',code:`# 🎓 المشروع الختامي: نظام إدارة مشاريع
# يجمع: أصناف، وراثة، بنيات، تعدادات، طابق، لامدا، f-strings

تعداد حالة_المهمة
    جديدة
    قيد_العمل
    مكتملة
    معلقة
نهاية

تعداد أولوية
    منخفضة
    عادية
    عالية
    حرجة
نهاية

بنية مدة
    ساعات
    دقائق
نهاية

صنف مهمة
    باني(العنوان: نص، الأولوية: رقم)
        هذا.العنوان = العنوان
        هذا.الأولوية = الأولوية
        هذا.الحالة = حالة_المهمة.جديدة
        هذا.الوقت = جديد مدة(0، 0)
    نهاية

    دالة ابدأ()
        هذا.الحالة = حالة_المهمة.قيد_العمل
    نهاية

    دالة أكمل()
        هذا.الحالة = حالة_المهمة.مكتملة
    نهاية

    دالة رمز_الحالة(): نص
        طابق (هذا.الحالة)
            عندما 0:
                ارجع "🆕"
            عندما 1:
                ارجع "🔄"
            عندما 2:
                ارجع "✅"
            عندما 3:
                ارجع "⏸️"
            افتراضي:
                ارجع "❓"
        نهاية
    نهاية

    دالة رمز_الأولوية(): نص
        طابق (هذا.الأولوية)
            عندما 0:
                ارجع "🟢"
            عندما 1:
                ارجع "🟡"
            عندما 2:
                ارجع "🟠"
            عندما 3:
                ارجع "🔴"
            افتراضي:
                ارجع "⚪"
        نهاية
    نهاية

    دالة عرض()
        اطبع(f"{هذا.رمز_الحالة()} {هذا.رمز_الأولوية()} {هذا.العنوان}")
    نهاية
نهاية

صنف مشروع
    باني(الاسم: نص)
        هذا.الاسم = الاسم
        هذا.المهام = []
    نهاية

    دالة أضف(عنوان: نص، أولوية: رقم)
        هذا.المهام.أضف(جديد مهمة(عنوان، أولوية))
    نهاية

    دالة التقدم(): رقم
        إذا (طول(هذا.المهام) == 0)
            ارجع 0
        نهاية
        متغير مكتملة = 0
        لكل م في هذا.المهام
            إذا (م.الحالة == حالة_المهمة.مكتملة)
                مكتملة += 1
            نهاية
        نهاية
        ارجع (مكتملة * 100) / طول(هذا.المهام)
    نهاية

    دالة تقرير()
        اطبع(f"\\n📊 مشروع: {هذا.الاسم}")
        اطبع(f"التقدم: {هذا.التقدم()}%")
        اطبع("المهام:")
        لكل م في هذا.المهام
            م.عرض()
        نهاية
    نهاية
نهاية

# === التجربة ===
متغير مشروعي = جديد مشروع("موقع لغة ص")
مشروعي.أضف("تصميم الواجهة"، أولوية.عالية)
مشروعي.أضف("كتابة الدروس"، أولوية.حرجة)
مشروعي.أضف("إضافة أمثلة"، أولوية.عادية)
مشروعي.أضف("اختبار الأداء"، أولوية.منخفضة)

# محاكاة التقدم
مشروعي.المهام[0].ابدأ()
مشروعي.المهام[0].أكمل()
مشروعي.المهام[1].ابدأ()

مشروعي.تقرير()`,exercise:'أضف: فلترة بالأولوية، ترتيب حسب الحالة، صنف "فريق" يحتوي أعضاء ومشاريع.',hint:"دالة فلتر(الأولوية) تستخدم حلقة وتجمع المطابقة. صنف فريق يحتوي مصفوفة أعضاء ومشاريع.",tips:["🎓 هذا المشروع يجمع كل المستوى الثالث!","الأصناف + الوراثة + البنيات + التعدادات + طابق","لامدا + f-strings + تعليقات أنواع"],concepts:["مشروع ختامي","تكامل المفاهيم","إدارة مشاريع"]}]},mc={id:3,title:"المتوسط",titleEn:"Intermediate",description:"البرمجة الكائنية، مطابقة الأنماط، الوحدات، والدوال المتقدمة.",icon:"🚀",color:"#8b5cf6",sections:[rc,dc,_c,uc,pc,vc,hc,fc]},gc={id:"l4_design_patterns",title:"أنماط التصميم",icon:"🏛️",lessons:[{id:"l4_01",title:"نمط المفرد (Singleton)",desc:"ضمان وجود نسخة واحدة فقط من صنف معين في البرنامج.",explanation:"نمط المفرد يشبه وجود مفتاح واحد فقط للمكتب المشترك — الجميع يستخدم نفس المفتاح بدلاً من نسخ متعددة قد تسبب فوضى. هذا النمط ضروري عندما تحتاج لمورد مشترك مثل إعدادات التطبيق أو اتصال قاعدة البيانات. يمنع تعدد النسخ تعارض البيانات ويوفر الذاكرة.",code:`# نمط المفرد — نسخة واحدة فقط
صنف إعدادات
    باني()
        هذا.بيانات = {}
    نهاية

    دالة عيّن(مفتاح، قيمة)
        هذا.بيانات[مفتاح] = قيمة
    نهاية

    دالة اقرأ(مفتاح)
        ارجع هذا.بيانات[مفتاح]
    نهاية
نهاية

# حفظ النسخة الوحيدة في متغير عام
متغير _نسخة = لاشيء

دالة احصل_إعدادات()
    إذا (_نسخة == لاشيء)
        _نسخة = جديد إعدادات()
    نهاية
    ارجع _نسخة
نهاية

# الاستخدام — نفس النسخة دائماً
متغير إ1 = احصل_إعدادات()
إ1.عيّن("لغة"، "العربية")
متغير إ2 = احصل_إعدادات()
اطبع(إ2.اقرأ("لغة"))  # العربية`,exercise:'أنشئ نمط مفرد لصنف "سجل" يحفظ رسائل السجل في مصفوفة ويوفر دالة لعرضها.',hint:"استخدم دالة خارجية تتحقق من وجود النسخة قبل إنشاء واحدة جديدة.",tips:["نمط المفرد يضمن نسخة واحدة من الصنف","نستخدم دالة مصنع بدلاً من جديد مباشرة","مفيد للإعدادات والسجلات والاتصالات"],concepts:["نمط المفرد","Singleton","دالة مصنع"]},{id:"l4_02",title:"نمط المصنع (Factory)",desc:"إنشاء كائنات بناءً على نوع أو شرط دون كشف تفاصيل الإنشاء.",explanation:'تخيل مصنع سيارات لا يهمك تفاصيل تجميعه — تطلب "سيارة رياضية" فيسلمك واحدة جاهزة. نمط المصنع يخفي تعقيد إنشاء الكائنات ويتيح لك إضافة أنواع جديدة لاحقاً دون تعديل الكود المستدعي. هذا يجعل برنامجك أكثر مرونة وقابلية للتوسع.',code:`# نمط المصنع — إنشاء كائنات حسب النوع
صنف زر
    باني(نص)
        هذا.نص = نص
        هذا.نوع = "عادي"
    نهاية
    دالة اعرض()
        اطبع("[" + هذا.نص + "]")
    نهاية
نهاية

صنف زر_خطر يرث زر
    باني(نص)
        الأساس(نص)
        هذا.نوع = "خطر"
    نهاية
    دالة اعرض()
        اطبع("⚠️ [" + هذا.نص + "] ⚠️")
    نهاية
نهاية

صنف زر_نجاح يرث زر
    باني(نص)
        الأساس(نص)
        هذا.نوع = "نجاح"
    نهاية
    دالة اعرض()
        اطبع("✅ [" + هذا.نص + "]")
    نهاية
نهاية

# دالة المصنع
دالة أنشئ_زر(نوع، نص)
    طابق (نوع)
        عندما "خطر":
            ارجع جديد زر_خطر(نص)
        عندما "نجاح":
            ارجع جديد زر_نجاح(نص)
        افتراضي:
            ارجع جديد زر(نص)
    نهاية
نهاية

متغير ز1 = أنشئ_زر("خطر"، "حذف")
متغير ز2 = أنشئ_زر("نجاح"، "حفظ")
ز1.اعرض()
ز2.اعرض()`,exercise:"أنشئ مصنع أشكال هندسية (دائرة، مربع، مثلث) كل منها يحسب المساحة.",hint:'أنشئ صنف أساسي "شكل" وأصنافاً فرعية، ثم دالة مصنع تختار الصنف المناسب.',tips:["المصنع يخفي تعقيد الإنشاء","الكود المستدعي لا يحتاج معرفة الأصناف الفرعية","يسهّل إضافة أنواع جديدة لاحقاً"],concepts:["نمط المصنع","Factory","وراثة","طابق"]},{id:"l4_03",title:"نمط المراقب (Observer)",desc:"إخطار عدة كائنات تلقائياً عند حدوث تغيير في كائن آخر.",explanation:"نمط المراقب يعمل مثل الاشتراك في قناة يوتيوب — عندما يُنشر محتوى جديد، يُخطر جميع المشتركين تلقائياً دون أن يحتاج الناشر لمعرفة من هم. هذا النمط أساسي في واجهات المستخدم حيث تتغير البيانات في مكان وتُحدَّث الشاشة في مكان آخر. يفصل مصدر الأحداث عن المستقبلين مما يجعل الكود أقل ترابطاً.",code:`# نمط المراقب — الإشعارات التلقائية
صنف ناشر
    باني()
        هذا.مراقبون = []
    نهاية

    دالة اشترك(مراقب)
        هذا.مراقبون.أضف(مراقب)
    نهاية

    دالة أخبر(حدث)
        لكل م في هذا.مراقبون
            م(حدث)
        نهاية
    نهاية
نهاية

صنف متجر يرث ناشر
    باني()
        الأساس()
        هذا.منتجات = []
    نهاية

    دالة أضف_منتج(منتج)
        هذا.منتجات.أضف(منتج)
        هذا.أخبر("منتج جديد: " + منتج)
    نهاية
نهاية

# إنشاء مراقبين (دوال)
متغير المتجر = جديد متجر()
المتجر.اشترك(لامدا (حدث) اطبع("📧 بريد: " + حدث) نهاية)
المتجر.اشترك(لامدا (حدث) اطبع("📱 إشعار: " + حدث) نهاية)

المتجر.أضف_منتج("هاتف")
المتجر.أضف_منتج("حاسوب")`,exercise:"أنشئ نظام إشعارات لتطبيق أخبار — عند نشر خبر جديد يتم إخطار جميع المشتركين.",hint:"استخدم مصفوفة لحفظ دوال المراقبين واستدعِها عند كل حدث.",tips:["نمط المراقب يفصل الناشر عن المشتركين","اللامدا مثالية كدوال مراقبة خفيفة","مستخدم في واجهات المستخدم وأنظمة الأحداث"],concepts:["نمط المراقب","Observer","أحداث","لامدا"]},{id:"l4_04",title:"نمط الاستراتيجية (Strategy)",desc:"تبديل خوارزمية في وقت التشغيل بتغليفها في كائنات مستقلة.",explanation:'فكر في GPS يتيح لك اختيار "أسرع طريق" أو "أقصر مسافة" أو "تجنب الرسوم" — كلها تصل لنفس الوجهة لكن بطرق مختلفة. نمط الاستراتيجية يغلف كل طريقة في كائن مستقل ويتيح تبديلها دون تغيير الكود الرئيسي. مفيد جداً لخوارزميات الترتيب وقواعد التسعير والدفع.',code:`# نمط الاستراتيجية — تبديل السلوك ديناميكياً
دالة ترتيب_تصاعدي(أ، ب)
    ارجع أ - ب
نهاية

دالة ترتيب_تنازلي(أ، ب)
    ارجع ب - أ
نهاية

صنف قائمة_مرتبة
    باني(عناصر)
        هذا.عناصر = عناصر
        هذا.استراتيجية = ترتيب_تصاعدي
    نهاية

    دالة غيّر_ترتيب(استراتيجية)
        هذا.استراتيجية = استراتيجية
    نهاية

    دالة رتّب()
        # ترتيب فقاعي بالاستراتيجية المختارة
        متغير ن = هذا.عناصر.طول()
        لكل ت في [0..ن - 1]
            لكل ج في [0..ن - ت - 2]
                إذا (هذا.استراتيجية(هذا.عناصر[ج]، هذا.عناصر[ج + 1]) > 0)
                    متغير مؤقت = هذا.عناصر[ج]
                    هذا.عناصر[ج] = هذا.عناصر[ج + 1]
                    هذا.عناصر[ج + 1] = مؤقت
                نهاية
            نهاية
        نهاية
    نهاية

    دالة اعرض()
        اطبع(هذا.عناصر)
    نهاية
نهاية

متغير ق = جديد قائمة_مرتبة([5، 2، 8، 1، 9])
ق.رتّب()
ق.اعرض()  # [1, 2, 5, 8, 9]

ق.غيّر_ترتيب(ترتيب_تنازلي)
ق.رتّب()
ق.اعرض()  # [9, 8, 5, 2, 1]`,exercise:"أنشئ نظام خصومات بثلاث استراتيجيات: نسبة مئوية، مبلغ ثابت، واشتر واحد واحصل الثاني مجاناً.",hint:"اجعل كل استراتيجية دالة تأخذ السعر وترجع السعر بعد الخصم.",tips:["الاستراتيجية تغلف خوارزمية في دالة أو كائن","يمكن تبديلها بسهولة وقت التشغيل","الدوال من الدرجة الأولى تجعل التطبيق أبسط"],concepts:["نمط الاستراتيجية","Strategy","دوال عليا"]},{id:"l4_05",title:"نمط الحالة (State)",desc:"تغيير سلوك الكائن عندما تتغير حالته الداخلية.",explanation:'مثل مصباح الإشارة: أحمر يعني "قف"، أخضر يعني "سر"، أصفر يعني "استعد". نفس الضغط على الزر يعطي نتيجة مختلفة حسب الحالة الحالية. نمط الحالة ينظم السلوك المعقد بتقسيمه إلى حالات واضحة مع انتقالات محددة. ضروري لآلات البيع، طلبات الشراء، ومشغلات الوسائط.',code:`# نمط الحالة — سلوك يتغير حسب الحالة
صنف مشغل_موسيقى
    باني()
        هذا.حالة = "متوقف"
        هذا.أغنية = ""
    نهاية

    دالة شغّل(أغنية)
        طابق (هذا.حالة)
            عندما "متوقف":
                هذا.أغنية = أغنية
                هذا.حالة = "يعمل"
                اطبع("▶️ تشغيل: " + أغنية)
            عندما "يعمل":
                اطبع("⏸ إيقاف مؤقت أولاً")
            عندما "مؤقت":
                هذا.حالة = "يعمل"
                اطبع("▶️ استئناف: " + هذا.أغنية)
        نهاية
    نهاية

    دالة أوقف_مؤقت()
        إذا (هذا.حالة == "يعمل")
            هذا.حالة = "مؤقت"
            اطبع("⏸ إيقاف مؤقت")
        وإلا
            اطبع("ليس قيد التشغيل")
        نهاية
    نهاية

    دالة أوقف()
        هذا.حالة = "متوقف"
        هذا.أغنية = ""
        اطبع("⏹ توقف")
    نهاية
نهاية

متغير م = جديد مشغل_موسيقى()
م.شغّل("أغنية الصباح")
م.أوقف_مؤقت()
م.شغّل("")
م.أوقف()`,exercise:"أنشئ نظام طلب بحالات: جديد، قيد_التجهيز، مشحون، مُسلّم. كل حالة تسمح بإجراءات مختلفة.",hint:"استخدم طابق للتحقق من الحالة الحالية وتحديد الإجراءات المسموحة.",tips:["نمط الحالة يجعل الانتقالات واضحة ومنظمة","طابق/عندما مثالي لإدارة الحالات","كل حالة تحدد السلوك والانتقالات الممكنة"],concepts:["نمط الحالة","State","آلة حالة","طابق"]},{id:"l4_06",title:"نمط المُزخرف (Decorator)",desc:"إضافة سلوك جديد لكائن ديناميكياً دون تعديل صنفه الأصلي.",explanation:"مثل طلب قهوة يمكنك إضافة حليب أو سكر أو كريمة — كل إضافة تغلف الأصل وتضيف شيئاً دون تغييره. المُزخرف بديل مرن للوراثة خاصة عندما تحتاج تركيبات متعددة من السلوكيات. يُستخدم كثيراً في تدفقات الإدخال/الإخراج وأنظمة الصلاحيات والتسجيل.",code:`# نمط المُزخرف — تغليف وإضافة وظائف
صنف قهوة
    باني()
        هذا.الوصف = "قهوة عادية"
        هذا.السعر = 10
    نهاية
    دالة اعرض()
        اطبع(هذا.الوصف + " — " + هذا.السعر + " ريال")
    نهاية
نهاية

# دوال تزيين تلف الكائن
دالة أضف_حليب(قهوة)
    قهوة.الوصف = قهوة.الوصف + " + حليب"
    قهوة.السعر = قهوة.السعر + 3
    ارجع قهوة
نهاية

دالة أضف_سكر(قهوة)
    قهوة.الوصف = قهوة.الوصف + " + سكر"
    قهوة.السعر = قهوة.السعر + 1
    ارجع قهوة
نهاية

دالة أضف_كريمة(قهوة)
    قهوة.الوصف = قهوة.الوصف + " + كريمة"
    قهوة.السعر = قهوة.السعر + 5
    ارجع قهوة
نهاية

# تركيب الزخارف
متغير طلبي = جديد قهوة()
طلبي = أضف_حليب(طلبي)
طلبي = أضف_سكر(طلبي)
طلبي = أضف_كريمة(طلبي)
طلبي.اعرض()  # قهوة عادية + حليب + سكر + كريمة — 19 ريال`,exercise:"أنشئ نظام تزيين لرسالة نصية: إضافة تشفير، إضافة طابع زمني، إضافة توقيع.",hint:"أنشئ دوال تأخذ كائن الرسالة وتضيف له خصائص جديدة ثم ترجعه.",tips:["المُزخرف يضيف وظائف دون تغيير الصنف الأصلي","يمكن تركيب عدة زخارف فوق بعضها","بديل مرن عن كثرة الوراثة"],concepts:["نمط المُزخرف","Decorator","تغليف"]},{id:"l4_07",title:"نمط الأمر (Command)",desc:"تغليف العمليات ككائنات قابلة للتنفيذ والتراجع.",explanation:"عندما تضغط Ctrl+Z للتراجع في محرر نص، ينفذ نمط الأمر. كل عملية (كتابة، حذف، نسخ) تُغلَّف ككائن يعرف كيف ينفذ نفسه وكيف يلغي نفسه. هذا يتيح بناء سجل عمليات كامل، ودعم التراجع/الإعادة، وتنفيذ الماكرو.",code:`# نمط الأمر — عمليات قابلة للتراجع
صنف محرر_نص
    باني()
        هذا.نص = ""
        هذا.تاريخ = []
    نهاية

    دالة نفّذ(أمر)
        أمر.نفّذ(هذا)
        هذا.تاريخ.أضف(أمر)
    نهاية

    دالة تراجع()
        إذا (هذا.تاريخ.طول() > 0)
            متغير آخر = هذا.تاريخ[هذا.تاريخ.طول() - 1]
            آخر.ألغِ(هذا)
            # إزالة آخر عنصر
            هذا.تاريخ = هذا.تاريخ.شريحة(0، هذا.تاريخ.طول() - 1)
        نهاية
    نهاية
نهاية

صنف أمر_إضافة
    باني(نص)
        هذا.نص = نص
    نهاية
    دالة نفّذ(محرر)
        محرر.نص = محرر.نص + هذا.نص
    نهاية
    دالة ألغِ(محرر)
        متغير ط = محرر.نص.طول() - هذا.نص.طول()
        محرر.نص = محرر.نص.جزء(0، ط)
    نهاية
نهاية

متغير م = جديد محرر_نص()
م.نفّذ(جديد أمر_إضافة("مرحباً "))
م.نفّذ(جديد أمر_إضافة("بالعالم"))
اطبع(م.نص)  # مرحباً بالعالم
م.تراجع()
اطبع(م.نص)  # مرحباً`,exercise:"أنشئ آلة حاسبة بنمط الأمر تدعم الجمع والطرح والضرب مع إمكانية التراجع.",hint:"كل عملية حسابية تكون كائناً بدالتي نفّذ() وألغِ().",tips:["نمط الأمر يفصل الطلب عن التنفيذ","يتيح التراجع (Undo) بسهولة","مفيد في المحررات والألعاب"],concepts:["نمط الأمر","Command","تراجع","تاريخ"]},{id:"l4_08",title:"نمط الباني (Builder)",desc:"بناء كائنات معقدة خطوة بخطوة مع تسلسل الدوال.",explanation:'مثل بناء شطيرة في مطعم: تختار الخبز، ثم اللحم، ثم الخضار، ثم الصلصة خطوة بخطوة. كل خطوة ترجع "هذا" للسماح بالتسلسل. الباني مثالي عندما تملك كائناً بعشرات الإعدادات الاختيارية — بدلاً من باني واحد بـ 20 معامل، لديك دوال واضحة لكل خيار.',code:`# نمط الباني — بناء تدريجي متسلسل
صنف باني_استعلام
    باني()
        هذا.جدول = ""
        هذا.شروط = []
        هذا.حقول = "*"
        هذا.حد = 0
    نهاية

    دالة من(جدول)
        هذا.جدول = جدول
        ارجع هذا
    نهاية

    دالة اختر(حقول)
        هذا.حقول = حقول
        ارجع هذا
    نهاية

    دالة أين(شرط)
        هذا.شروط.أضف(شرط)
        ارجع هذا
    نهاية

    دالة حدد(عدد)
        هذا.حد = عدد
        ارجع هذا
    نهاية

    دالة ابنِ()
        متغير ن = "SELECT " + هذا.حقول + " FROM " + هذا.جدول
        إذا (هذا.شروط.طول() > 0)
            متغير شروط_نص = ""
            لكل ش في هذا.شروط
                إذا (شروط_نص != "")
                    شروط_نص = شروط_نص + " AND "
                نهاية
                شروط_نص = شروط_نص + ش
            نهاية
            ن = ن + " WHERE " + شروط_نص
        نهاية
        إذا (هذا.حد > 0)
            ن = ن + " LIMIT " + هذا.حد
        نهاية
        ارجع ن
    نهاية
نهاية

# بناء استعلام متسلسل
متغير استعلام = جديد باني_استعلام()
متغير نتيجة = استعلام.من("مستخدمون").اختر("الاسم، العمر").أين("العمر > 18").أين("نشط = 1").حدد(10).ابنِ()
اطبع(نتيجة)`,exercise:"أنشئ باني HTML يبني عناصر HTML متداخلة بتسلسل الدوال.",hint:"أنشئ دوال مثل عنصر()، محتوى()، خاصية()، ابنِ() كل منها ترجع هذا.",tips:["نمط الباني يبسّط إنشاء كائنات معقدة",'إرجاع "هذا" يتيح تسلسل الاستدعاءات',"يفصل عملية البناء عن التمثيل النهائي"],concepts:["نمط الباني","Builder","تسلسل","Fluent API"]},{id:"l4_09",title:"نمط MVC",desc:"فصل البيانات والعرض والتحكم في تطبيق متكامل.",explanation:"تخيل مطعماً: النموذج هو المطبخ (البيانات)، العرض هو قائمة الطعام والأطباق (ما يراه العميل)، والمتحكم هو النادل (ينسق بينهما). MVC يفصل المسؤوليات فيمكنك تغيير التصميم دون المساس بالمنطق، أو تغيير قاعدة البيانات دون إعادة بناء الواجهة. أساس معظم أطر عمل الويب الحديثة.",code:`# نمط MVC — Model-View-Controller
# النموذج: البيانات والمنطق
صنف نموذج_مهام
    باني()
        هذا.مهام = []
        هذا.مراقبون = []
    نهاية
    دالة أضف(مهمة)
        هذا.مهام.أضف({"نص": مهمة، "مكتمل": خطأ})
        هذا.أخبر()
    نهاية
    دالة أكمل(فهرس)
        هذا.مهام[فهرس]["مكتمل"] = صحيح
        هذا.أخبر()
    نهاية
    دالة اشترك(دالة_مراقب)
        هذا.مراقبون.أضف(دالة_مراقب)
    نهاية
    دالة أخبر()
        لكل م في هذا.مراقبون
            م(هذا.مهام)
        نهاية
    نهاية
نهاية

# العرض: الواجهة
صنف عرض_مهام
    باني()
    نهاية
    دالة اعرض(مهام)
        اطبع("═══ قائمة المهام ═══")
        متغير ت = 0
        لكل مهمة في مهام
            متغير حالة = "⬜"
            إذا (مهمة["مكتمل"])
                حالة = "✅"
            نهاية
            اطبع(حالة + " " + ت + ": " + مهمة["نص"])
            ت = ت + 1
        نهاية
    نهاية
نهاية

# المتحكم: الوسيط
صنف متحكم_مهام
    باني()
        هذا.نموذج = جديد نموذج_مهام()
        هذا.عرض = جديد عرض_مهام()
        هذا.نموذج.اشترك(لامدا (م) هذا.عرض.اعرض(م) نهاية)
    نهاية
    دالة أضف_مهمة(نص)
        هذا.نموذج.أضف(نص)
    نهاية
    دالة أكمل_مهمة(فهرس)
        هذا.نموذج.أكمل(فهرس)
    نهاية
نهاية

متغير تطبيق = جديد متحكم_مهام()
تطبيق.أضف_مهمة("تعلم أنماط التصميم")
تطبيق.أضف_مهمة("كتابة مشروع")
تطبيق.أكمل_مهمة(0)`,exercise:"حوّل تطبيق قائمة جهات الاتصال إلى نمط MVC مع إضافة وحذف وبحث.",hint:"أنشئ ثلاثة أصناف: نموذج للبيانات، عرض للطباعة، متحكم للتنسيق بينهما.",tips:["MVC يفصل المسؤوليات في التطبيق","النموذج لا يعرف شيئاً عن العرض","المتحكم ينسق بين النموذج والعرض"],concepts:["MVC","فصل المسؤوليات","نمط المراقب"]}]},xc={id:"l4_data_structures",title:"هياكل البيانات",icon:"🗄️",lessons:[{id:"l4_10",title:"القائمة المترابطة (Linked List)",desc:"بناء قائمة مترابطة من البداية — العقد والمؤشرات.",explanation:"تخيل قطاراً حيث كل عربة تعرف العربة التي خلفها فقط — هذه هي القائمة المترابطة. عكس المصفوفة الثابتة، يمكنك إضافة أو حذف عقد في أي مكان بسرعة. لكن الوصول لعنصر معين يتطلب المرور على كل ما قبله. مثالية للبيانات التي تُضاف وتُحذف كثيراً.",code:`# القائمة المترابطة — كل عقدة تشير للتالية
صنف عقدة
    باني(قيمة)
        هذا.قيمة = قيمة
        هذا.التالي = لاشيء
    نهاية
نهاية

صنف قائمة_مترابطة
    باني()
        هذا.الرأس = لاشيء
        هذا.الحجم = 0
    نهاية

    دالة أضف(قيمة)
        متغير جديدة = جديد عقدة(قيمة)
        إذا (هذا.الرأس == لاشيء)
            هذا.الرأس = جديدة
        وإلا
            متغير حالي = هذا.الرأس
            بينما (حالي.التالي != لاشيء)
                حالي = حالي.التالي
            نهاية
            حالي.التالي = جديدة
        نهاية
        هذا.الحجم = هذا.الحجم + 1
    نهاية

    دالة اعرض()
        متغير حالي = هذا.الرأس
        متغير نتيجة = ""
        بينما (حالي != لاشيء)
            نتيجة = نتيجة + حالي.قيمة
            إذا (حالي.التالي != لاشيء)
                نتيجة = نتيجة + " → "
            نهاية
            حالي = حالي.التالي
        نهاية
        اطبع(نتيجة)
    نهاية

    دالة ابحث(قيمة)
        متغير حالي = هذا.الرأس
        بينما (حالي != لاشيء)
            إذا (حالي.قيمة == قيمة)
                ارجع صحيح
            نهاية
            حالي = حالي.التالي
        نهاية
        ارجع خطأ
    نهاية
نهاية

متغير ق = جديد قائمة_مترابطة()
ق.أضف(10)
ق.أضف(20)
ق.أضف(30)
ق.اعرض()  # 10 → 20 → 30
اطبع(ق.ابحث(20))  # صحيح`,exercise:'أضف دالة "احذف" تحذف عقدة بقيمة محددة من القائمة المترابطة.',hint:"تعامل مع حالتين: حذف الرأس، وحذف عقدة في الوسط/النهاية.",tips:["القائمة المترابطة مرنة في الإضافة والحذف","كل عقدة تحمل قيمة ومؤشراً للعقدة التالية","التنقل يتم بتتبع سلسلة المؤشرات"],concepts:["قائمة مترابطة","عقدة","مؤشر","Linked List"]},{id:"l4_11",title:"المكدس (Stack)",desc:"بنية LIFO — آخر عنصر يدخل أول عنصر يخرج.",explanation:"المكدس مثل كومة أطباق — الطبق الأخير الذي وضعته هو أول ما تسحبه. هذه البنية جوهرية في البرمجة: استدعاء الدوال يستخدم مكدساً، التراجع (Undo) يستخدم مكدساً، تحليل الأقواس يستخدم مكدساً. عمليتان أساسيتان: ادفع (push) واسحب (pop).",code:`# المكدس — LIFO (آخر داخل أول خارج)
صنف مكدس
    باني()
        هذا.عناصر = []
    نهاية

    دالة ادفع(قيمة)
        هذا.عناصر.أضف(قيمة)
    نهاية

    دالة اسحب()
        إذا (هذا.فارغ())
            ارمي "المكدس فارغ!"
        نهاية
        متغير آخر = هذا.عناصر[هذا.عناصر.طول() - 1]
        هذا.عناصر = هذا.عناصر.شريحة(0، هذا.عناصر.طول() - 1)
        ارجع آخر
    نهاية

    دالة قمة()
        إذا (هذا.فارغ())
            ارجع لاشيء
        نهاية
        ارجع هذا.عناصر[هذا.عناصر.طول() - 1]
    نهاية

    دالة فارغ()
        ارجع هذا.عناصر.طول() == 0
    نهاية

    دالة الحجم()
        ارجع هذا.عناصر.طول()
    نهاية
نهاية

# تطبيق: فحص الأقواس المتوازنة
دالة أقواس_متوازنة(نص)
    متغير م = جديد مكدس()
    لكل حرف في نص.قسم("")
        إذا (حرف == "(" أو حرف == "[" أو حرف == "{")
            م.ادفع(حرف)
        وإلا
            إذا (حرف == ")" أو حرف == "]" أو حرف == "}")
                إذا (م.فارغ())
                    ارجع خطأ
                نهاية
                م.اسحب()
            نهاية
        نهاية
    نهاية
    ارجع م.فارغ()
نهاية

اطبع(أقواس_متوازنة("(أ + [ب * ج])"))  # صحيح
اطبع(أقواس_متوازنة("(أ + [ب)"))        # خطأ`,exercise:"استخدم المكدس لعكس نص (اسحب الحروف واحداً تلو الآخر).",hint:"ادفع كل حرف في المكدس، ثم اسحبها بالترتيب المعكوس.",tips:["المكدس يستخدم في تحليل التعبيرات والتراجع","العمليات الأساسية: ادفع (push) واسحب (pop)","LIFO: آخر عنصر يدخل أول من يخرج"],concepts:["مكدس","Stack","LIFO","أقواس متوازنة"]},{id:"l4_12",title:"الرتل (Queue)",desc:"بنية FIFO — أول عنصر يدخل أول عنصر يخرج.",explanation:"الرتل مثل طابور البنك — من أتى أولاً يُخدَم أولاً. ضروري لأي نظام يحافظ على العدالة في الترتيب: نظام الطباعة، جدولة المهام، رسائل التطبيقات. عمليتان: أدخل (enqueue) في النهاية وأخرج (dequeue) من البداية.",code:`# الرتل — FIFO (أول داخل أول خارج)
صنف رتل
    باني()
        هذا.عناصر = []
    نهاية

    دالة أدخل(قيمة)
        هذا.عناصر.أضف(قيمة)
    نهاية

    دالة أخرج()
        إذا (هذا.فارغ())
            ارمي "الرتل فارغ!"
        نهاية
        متغير أول = هذا.عناصر[0]
        هذا.عناصر = هذا.عناصر.شريحة(1)
        ارجع أول
    نهاية

    دالة الأمام()
        إذا (هذا.فارغ())
            ارجع لاشيء
        نهاية
        ارجع هذا.عناصر[0]
    نهاية

    دالة فارغ()
        ارجع هذا.عناصر.طول() == 0
    نهاية

    دالة الحجم()
        ارجع هذا.عناصر.طول()
    نهاية
نهاية

# محاكاة طابور عملاء
متغير طابور = جديد رتل()
طابور.أدخل("أحمد")
طابور.أدخل("سارة")
طابور.أدخل("محمد")

بينما (!طابور.فارغ())
    اطبع("خدمة العميل: " + طابور.أخرج())
نهاية`,exercise:"أنشئ نظام طباعة بسيط — مستندات تنتظر في رتل وتُطبع بالترتيب.",hint:"أدخل كل مستند ككائن يحتوي على اسم وعدد صفحات في الرتل.",tips:["الرتل يحافظ على ترتيب الوصول","FIFO: أول من يدخل أول من يخرج","مستخدم في جدولة المهام والطوابير"],concepts:["رتل","Queue","FIFO","جدولة"]},{id:"l4_13",title:"الشجرة الثنائية (Binary Tree)",desc:"بناء شجرة ثنائية — كل عقدة لها ابنان كحد أقصى.",explanation:'الشجرة الثنائية للبحث تشبه لعبة "أكبر أم أصغر" — كل سؤال يقطع الإمكانيات للنصف. هذا يجعل البحث سريعاً جداً O(log n). الأشجار أساسية في قواعد البيانات، محرر النصوص، وأنظمة الملفات. العودية طريقة طبيعية للتعامل معها.',code:`# الشجرة الثنائية — بحث فعال
صنف عقدة_شجرة
    باني(قيمة)
        هذا.قيمة = قيمة
        هذا.يسار = لاشيء
        هذا.يمين = لاشيء
    نهاية
نهاية

صنف شجرة_بحث
    باني()
        هذا.الجذر = لاشيء
    نهاية

    دالة أضف(قيمة)
        هذا.الجذر = هذا._أضف(هذا.الجذر، قيمة)
    نهاية

    دالة _أضف(عقدة، قيمة)
        إذا (عقدة == لاشيء)
            ارجع جديد عقدة_شجرة(قيمة)
        نهاية
        إذا (قيمة < عقدة.قيمة)
            عقدة.يسار = هذا._أضف(عقدة.يسار، قيمة)
        وإلا
            عقدة.يمين = هذا._أضف(عقدة.يمين، قيمة)
        نهاية
        ارجع عقدة
    نهاية

    دالة ابحث(قيمة)
        ارجع هذا._ابحث(هذا.الجذر، قيمة)
    نهاية

    دالة _ابحث(عقدة، قيمة)
        إذا (عقدة == لاشيء)
            ارجع خطأ
        نهاية
        إذا (قيمة == عقدة.قيمة)
            ارجع صحيح
        وإلا
            إذا (قيمة < عقدة.قيمة)
                ارجع هذا._ابحث(عقدة.يسار، قيمة)
            وإلا
                ارجع هذا._ابحث(عقدة.يمين، قيمة)
            نهاية
        نهاية
    نهاية

    دالة ترتيب_وسطي(عقدة)
        إذا (عقدة == لاشيء)
            ارجع []
        نهاية
        متغير نتيجة = هذا.ترتيب_وسطي(عقدة.يسار)
        نتيجة.أضف(عقدة.قيمة)
        متغير يمين = هذا.ترتيب_وسطي(عقدة.يمين)
        لكل ق في يمين
            نتيجة.أضف(ق)
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير ش = جديد شجرة_بحث()
ش.أضف(50)
ش.أضف(30)
ش.أضف(70)
ش.أضف(20)
ش.أضف(40)
اطبع(ش.ابحث(30))  # صحيح
اطبع(ش.ترتيب_وسطي(ش.الجذر))  # [20, 30, 40, 50, 70]`,exercise:"أضف دالة لحساب ارتفاع الشجرة ودالة لعدّ العقد.",hint:"الارتفاع = أقصى ارتفاع بين اليسار واليمين + 1. عدد العقد = 1 + عقد_يسار + عقد_يمين.",tips:["الشجرة الثنائية تتيح البحث بـ O(log n)","الترتيب الوسطي يعطي العناصر مرتبة","العودية (recursion) طبيعية مع الأشجار"],concepts:["شجرة ثنائية","Binary Tree","عودية","بحث"]},{id:"l4_14",title:"جدول التجزئة (Hash Table)",desc:"بناء جدول تجزئة بسيط للوصول السريع بالمفاتيح.",explanation:"تخيل خزانة مكتب بريد — كل صندوق له رقم يُحسب من الاسم، فتصل مباشرة لصندوقك دون بحث. جدول التجزئة يوفر وصولاً شبه فوري O(1) عبر دالة تربط أي مفتاح بموقع محدد. الخرائط في أغلب اللغات تستخدم هذه البنية داخلياً.",code:`# جدول تجزئة — وصول سريع O(1) تقريباً
صنف جدول_تجزئة
    باني(حجم)
        هذا.حجم = حجم
        هذا.سلال = []
        لكل ت في [0..حجم - 1]
            هذا.سلال.أضف([])
        نهاية
    نهاية

    دالة _تجزئة(مفتاح)
        متغير مجموع = 0
        متغير أحرف = (مفتاح + "").قسم("")
        لكل ح في أحرف
            مجموع = مجموع + 1
        نهاية
        ارجع مجموع % هذا.حجم
    نهاية

    دالة عيّن(مفتاح، قيمة)
        متغير فهرس = هذا._تجزئة(مفتاح)
        متغير سلة = هذا.سلال[فهرس]
        لكل ت في [0..سلة.طول() - 1]
            إذا (سلة[ت][0] == مفتاح)
                سلة[ت] = [مفتاح، قيمة]
                ارجع "تم التحديث"
            نهاية
        نهاية
        سلة.أضف([مفتاح، قيمة])
    نهاية

    دالة اقرأ(مفتاح)
        متغير فهرس = هذا._تجزئة(مفتاح)
        متغير سلة = هذا.سلال[فهرس]
        لكل زوج في سلة
            إذا (زوج[0] == مفتاح)
                ارجع زوج[1]
            نهاية
        نهاية
        ارجع لاشيء
    نهاية
نهاية

متغير ج = جديد جدول_تجزئة(10)
ج.عيّن("اسم"، "أحمد")
ج.عيّن("عمر"، 25)
ج.عيّن("مدينة"، "الرياض")
اطبع(ج.اقرأ("اسم"))    # أحمد
اطبع(ج.اقرأ("مدينة"))  # الرياض`,exercise:'أضف دالة "احذف" لحذف مفتاح، ودالة "المفاتيح" لإرجاع جميع المفاتيح.',hint:"للحذف ابحث في السلة واحذف الزوج. للمفاتيح امسح كل السلال واجمع العنصر الأول من كل زوج.",tips:["جدول التجزئة يوفر وصولاً شبه فوري","التصادمات تُعالَج بالسلاسل (chaining)","دالة التجزئة تحوّل المفتاح إلى فهرس"],concepts:["جدول تجزئة","Hash Table","تجزئة","تصادم"]},{id:"l4_15",title:"الرسم البياني (Graph)",desc:"تمثيل الرسم البياني بقائمة المجاورة والتنقل فيه.",explanation:"الرسم البياني يمثل أي شبكة علاقات: الطرق بين المدن، الصداقات على فيسبوك، الروابط بين صفحات الويب. كل نقطة (عقدة) ترتبط بنقاط أخرى (حواف). قائمة المجاورة تحفظ جيران كل عقدة بكفاءة. أساس خرائط جوجل وتوصيات نتفلكس.",code:`# رسم بياني — قائمة مجاورة
صنف رسم_بياني
    باني()
        هذا.عقد = {}
    نهاية

    دالة أضف_عقدة(اسم)
        إذا (هذا.عقد[اسم] == لاشيء)
            هذا.عقد[اسم] = []
        نهاية
    نهاية

    دالة أضف_حافة(من، إلى)
        هذا.أضف_عقدة(من)
        هذا.أضف_عقدة(إلى)
        هذا.عقد[من].أضف(إلى)
        هذا.عقد[إلى].أضف(من)
    نهاية

    دالة الجيران(عقدة)
        ارجع هذا.عقد[عقدة]
    نهاية

    دالة اعرض()
        لكل مفتاح في هذا.عقد
            اطبع(مفتاح + " → " + هذا.عقد[مفتاح])
        نهاية
    نهاية
نهاية

متغير خريطة = جديد رسم_بياني()
خريطة.أضف_حافة("الرياض"، "جدة")
خريطة.أضف_حافة("الرياض"، "الدمام")
خريطة.أضف_حافة("جدة"، "المدينة")
خريطة.اعرض()
اطبع("جيران الرياض: " + خريطة.الجيران("الرياض"))`,exercise:"أضف دالة BFS (بحث بالعرض أولاً) للعثور على مسار بين عقدتين.",hint:"استخدم رتل للعقد المراد زيارتها وخريطة للعقد المزارة.",tips:["الرسم البياني يمثل علاقات بين كيانات","قائمة المجاورة فعالة في الذاكرة","يستخدم في الشبكات والخرائط والعلاقات"],concepts:["رسم بياني","Graph","قائمة مجاورة","حافة"]},{id:"l4_16",title:"رتل الأولوية (Priority Queue)",desc:"رتل يخرج العنصر ذي الأولوية الأعلى أولاً.",explanation:"في غرفة الطوارئ الحالة الحرجة تُعالج أولاً حتى لو أتت أخيراً — هذا رتل الأولوية. كل عنصر يحمل درجة أهمية، والأعلى يخرج أولاً. يستخدم في جدولة العمليات الحرجة، خوارزميات أقصر مسار (Dijkstra)، وأنظمة التنبيهات.",code:`# رتل الأولوية — الأعلى أولوية يخرج أولاً
صنف رتل_أولوية
    باني()
        هذا.عناصر = []
    نهاية

    دالة أدخل(قيمة، أولوية)
        هذا.عناصر.أضف({"قيمة": قيمة، "أولوية": أولوية})
        هذا._رتب()
    نهاية

    دالة _رتب()
        # ترتيب حسب الأولوية (الأعلى أولاً)
        متغير ن = هذا.عناصر.طول()
        لكل ت في [0..ن - 1]
            لكل ج في [0..ن - ت - 2]
                إذا (هذا.عناصر[ج]["أولوية"] < هذا.عناصر[ج + 1]["أولوية"])
                    متغير مؤقت = هذا.عناصر[ج]
                    هذا.عناصر[ج] = هذا.عناصر[ج + 1]
                    هذا.عناصر[ج + 1] = مؤقت
                نهاية
            نهاية
        نهاية
    نهاية

    دالة أخرج()
        إذا (هذا.عناصر.طول() == 0)
            ارمي "الرتل فارغ!"
        نهاية
        متغير أول = هذا.عناصر[0]
        هذا.عناصر = هذا.عناصر.شريحة(1)
        ارجع أول
    نهاية

    دالة الحجم()
        ارجع هذا.عناصر.طول()
    نهاية
نهاية

# نظام طوارئ
متغير طوارئ = جديد رتل_أولوية()
طوارئ.أدخل("صداع"، 1)
طوارئ.أدخل("كسر"، 5)
طوارئ.أدخل("حساسية"، 3)
طوارئ.أدخل("حالة حرجة"، 10)

بينما (طوارئ.الحجم() > 0)
    متغير حالة = طوارئ.أخرج()
    اطبع("معالجة: " + حالة["قيمة"] + " (أولوية " + حالة["أولوية"] + ")")
نهاية`,exercise:"أنشئ نظام جدولة مهام بأولويات — المهام العاجلة تُنفذ أولاً.",hint:"استخدم رتل الأولوية مع مهام لكل منها اسم ومستوى أولوية.",tips:["رتل الأولوية يخرج الأهم أولاً","مفيد في أنظمة الجدولة والطوارئ","يمكن تحسينه بالكومة (Heap) لأداء أفضل"],concepts:["رتل أولوية","Priority Queue","جدولة"]},{id:"l4_17",title:"المجموعة (Set)",desc:"بناء مجموعة تمنع التكرار وتدعم عمليات المجموعات.",explanation:"المجموعة مثل قائمة حضور ترفض تسجيل نفس الشخص مرتين. كل عنصر فريد — لا تكرار. عمليات المجموعات (اتحاد، تقاطع، فرق) مفيدة جداً في مقارنة البيانات: من المشتركين شاهدوا كلا الفيلمين؟ ما المنتجات الموجودة هنا وليست هناك؟",code:`# المجموعة — عناصر فريدة بدون تكرار
صنف مجموعة
    باني()
        هذا.عناصر = []
    نهاية

    دالة أضف(قيمة)
        إذا (!هذا.يحتوي(قيمة))
            هذا.عناصر.أضف(قيمة)
        نهاية
    نهاية

    دالة يحتوي(قيمة)
        ارجع هذا.عناصر.يحتوي(قيمة)
    نهاية

    دالة احذف(قيمة)
        متغير جديدة = []
        لكل ع في هذا.عناصر
            إذا (ع != قيمة)
                جديدة.أضف(ع)
            نهاية
        نهاية
        هذا.عناصر = جديدة
    نهاية

    دالة اتحاد(أخرى)
        متغير نتيجة = جديد مجموعة()
        لكل ع في هذا.عناصر
            نتيجة.أضف(ع)
        نهاية
        لكل ع في أخرى.عناصر
            نتيجة.أضف(ع)
        نهاية
        ارجع نتيجة
    نهاية

    دالة تقاطع(أخرى)
        متغير نتيجة = جديد مجموعة()
        لكل ع في هذا.عناصر
            إذا (أخرى.يحتوي(ع))
                نتيجة.أضف(ع)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية

    دالة اعرض()
        اطبع("{" + هذا.عناصر + "}")
    نهاية
نهاية

متغير أ = جديد مجموعة()
أ.أضف(1)
أ.أضف(2)
أ.أضف(3)
أ.أضف(2)  # لا يُضاف — موجود سابقاً

متغير ب = جديد مجموعة()
ب.أضف(2)
ب.أضف(3)
ب.أضف(4)

أ.اتحاد(ب).اعرض()   # {1, 2, 3, 4}
أ.تقاطع(ب).اعرض()  # {2, 3}`,exercise:'أضف دالة "فرق" تعطي العناصر الموجودة في المجموعة الأولى فقط.',hint:"الفرق = عناصر الأولى التي ليست في الثانية.",tips:["المجموعة تمنع تكرار العناصر","عمليات المجموعات: اتحاد، تقاطع، فرق","مفيدة لإزالة التكرارات والمقارنات"],concepts:["مجموعة","Set","اتحاد","تقاطع","فرق"]},{id:"l4_18",title:"المخزن الدائري (Circular Buffer)",desc:"مخزن بحجم ثابت يعيد الكتابة على الأقدم عند الامتلاء.",explanation:"مثل سجل كاميرا المراقبة الذي يحفظ آخر 24 ساعة فقط — القديم يُمسح ليفسح للجديد. المخزن الدائري يستخدم ذاكرة ثابتة ويعود للبداية عند الوصول للنهاية. مثالي للسجلات وبيانات الاستشعار ومعالجة الصوت في الوقت الحقيقي.",code:`# المخزن الدائري — حجم ثابت يعيد الاستخدام
صنف مخزن_دائري
    باني(سعة)
        هذا.سعة = سعة
        هذا.بيانات = []
        لكل ت في [0..سعة - 1]
            هذا.بيانات.أضف(لاشيء)
        نهاية
        هذا.رأس = 0
        هذا.عدد = 0
    نهاية

    دالة اكتب(قيمة)
        متغير فهرس = (هذا.رأس + هذا.عدد) % هذا.سعة
        إذا (هذا.عدد == هذا.سعة)
            # الكتابة فوق الأقدم
            هذا.بيانات[هذا.رأس] = قيمة
            هذا.رأس = (هذا.رأس + 1) % هذا.سعة
        وإلا
            هذا.بيانات[فهرس] = قيمة
            هذا.عدد = هذا.عدد + 1
        نهاية
    نهاية

    دالة اقرأ()
        إذا (هذا.عدد == 0)
            ارمي "المخزن فارغ!"
        نهاية
        متغير قيمة = هذا.بيانات[هذا.رأس]
        هذا.رأس = (هذا.رأس + 1) % هذا.سعة
        هذا.عدد = هذا.عدد - 1
        ارجع قيمة
    نهاية

    دالة اعرض()
        متغير نتيجة = []
        لكل ت في [0..هذا.عدد - 1]
            متغير فهرس = (هذا.رأس + ت) % هذا.سعة
            نتيجة.أضف(هذا.بيانات[فهرس])
        نهاية
        اطبع("مخزن: " + نتيجة)
    نهاية
نهاية

# سجل آخر 3 قراءات حرارة
متغير حرارة = جديد مخزن_دائري(3)
حرارة.اكتب(25)
حرارة.اكتب(27)
حرارة.اكتب(26)
حرارة.اعرض()  # [25, 27, 26]
حرارة.اكتب(30)  # يكتب فوق 25
حرارة.اعرض()  # [27, 26, 30]`,exercise:"أنشئ سجل أحداث دائري يحفظ آخر 5 أحداث مع طابع زمني.",hint:"أنشئ كائنات أحداث تحمل الوصف والوقت واستخدم المخزن الدائري.",tips:["المخزن الدائري مثالي للسجلات المحدودة","يمنع استهلاك ذاكرة غير محدود","مستخدم في السجلات والأنظمة المضمنة"],concepts:["مخزن دائري","Circular Buffer","سجلات"]}]},kc={id:"l4_algorithms",title:"الخوارزميات",icon:"⚙️",lessons:[{id:"l4_19",title:"الترتيب الفقاعي (Bubble Sort)",desc:"أبسط خوارزميات الترتيب — مقارنة وتبديل العناصر المتجاورة.",explanation:'الترتيب الفقاعي يشبه فقاعات الماء التي تصعد للسطح — العناصر الكبرى "تطفو" تدريجياً للنهاية. رغم بساطته، فهو بطيء O(n²) للبيانات الكبيرة. لكنه مثالي للتعلم لأن منطقه واضح: قارن جارين، بدلهما إذا لزم، كرر.',code:`# الترتيب الفقاعي — O(n²)
دالة ترتيب_فقاعي(مصفوفة)
    متغير ن = مصفوفة.طول()
    لكل ت في [0..ن - 1]
        متغير تبديل = خطأ
        لكل ج في [0..ن - ت - 2]
            إذا (مصفوفة[ج] > مصفوفة[ج + 1])
                متغير مؤقت = مصفوفة[ج]
                مصفوفة[ج] = مصفوفة[ج + 1]
                مصفوفة[ج + 1] = مؤقت
                تبديل = صحيح
            نهاية
        نهاية
        # إذا لم يحدث تبديل — المصفوفة مرتبة
        إذا (!تبديل)
            توقف
        نهاية
    نهاية
    ارجع مصفوفة
نهاية

# عرض خطوات الترتيب
دالة ترتيب_مرئي(مصفوفة)
    متغير ن = مصفوفة.طول()
    متغير خطوة = 0
    لكل ت في [0..ن - 1]
        لكل ج في [0..ن - ت - 2]
            إذا (مصفوفة[ج] > مصفوفة[ج + 1])
                متغير مؤقت = مصفوفة[ج]
                مصفوفة[ج] = مصفوفة[ج + 1]
                مصفوفة[ج + 1] = مؤقت
                خطوة = خطوة + 1
                اطبع("خطوة " + خطوة + ": " + مصفوفة)
            نهاية
        نهاية
    نهاية
نهاية

متغير أرقام = [64، 34، 25، 12، 22، 11، 90]
اطبع("قبل: " + أرقام)
ترتيب_مرئي(أرقام)
اطبع("بعد: " + أرقام)`,exercise:"عدّل الخوارزمية لترتيب مصفوفة أسماء أبجدياً.",hint:"المقارنة بين النصوص تعمل بنفس الطريقة — استخدم > و< للمقارنة.",tips:["الترتيب الفقاعي بسيط لكن بطيء O(n²)","تحسين: توقف مبكراً إذا لم يحدث تبديل","كل مرور يضع أكبر عنصر في مكانه"],concepts:["ترتيب فقاعي","Bubble Sort","O(n²)","تحسين"]},{id:"l4_20",title:"ترتيب الاختيار والإدراج",desc:"خوارزميتان إضافيتان: اختيار الأصغر، وإدراج في المكان الصحيح.",explanation:"ترتيب الاختيار كمن يبحث عن أقصر طالب ليقف أولاً، ثم ثاني أقصر، وهكذا. ترتيب الإدراج كمن يرتب ورق اللعب — يأخذ ورقة ويضعها في مكانها الصحيح بين المرتبة. الإدراج أفضل عملياً للبيانات المرتبة جزئياً.",code:`# ترتيب الاختيار — Selection Sort
دالة ترتيب_اختيار(مصفوفة)
    متغير ن = مصفوفة.طول()
    لكل ت في [0..ن - 2]
        متغير أصغر = ت
        لكل ج في [ت + 1..ن - 1]
            إذا (مصفوفة[ج] < مصفوفة[أصغر])
                أصغر = ج
            نهاية
        نهاية
        إذا (أصغر != ت)
            متغير مؤقت = مصفوفة[ت]
            مصفوفة[ت] = مصفوفة[أصغر]
            مصفوفة[أصغر] = مؤقت
        نهاية
    نهاية
    ارجع مصفوفة
نهاية

# ترتيب الإدراج — Insertion Sort
دالة ترتيب_إدراج(مصفوفة)
    متغير ن = مصفوفة.طول()
    لكل ت في [1..ن - 1]
        متغير مفتاح = مصفوفة[ت]
        متغير ج = ت - 1
        بينما (ج >= 0 و مصفوفة[ج] > مفتاح)
            مصفوفة[ج + 1] = مصفوفة[ج]
            ج = ج - 1
        نهاية
        مصفوفة[ج + 1] = مفتاح
    نهاية
    ارجع مصفوفة
نهاية

# مقارنة الخوارزميات
متغير بيانات1 = [29، 10، 14، 37، 13]
متغير بيانات2 = [29، 10، 14، 37، 13]
اطبع("اختيار: " + ترتيب_اختيار(بيانات1))
اطبع("إدراج: " + ترتيب_إدراج(بيانات2))`,exercise:"قارن عدد المقارنات بين الخوارزميات الثلاث على نفس المدخل.",hint:"أضف عدّاد مقارنات داخل كل خوارزمية وأرجعه مع النتيجة.",tips:["ترتيب الاختيار يجد الأصغر في كل مرور","ترتيب الإدراج فعال مع البيانات شبه المرتبة","كلاهما O(n²) لكن الإدراج أسرع عملياً"],concepts:["ترتيب الاختيار","ترتيب الإدراج","Selection Sort","Insertion Sort"]},{id:"l4_21",title:"ترتيب الدمج (Merge Sort)",desc:"خوارزمية فرّق تسد — قسّم وأدمج بكفاءة O(n log n).",explanation:'تخيل ترتيب أوراق امتحان: قسمها لنصفين، رتب كل نصف، ثم ادمجهما بالتناوب. هذه استراتيجية "فرّق تسد" القوية تضمن أداء O(n log n) دائماً — لا تتأثر بترتيب البيانات الأصلي. تكلفتها: مساحة إضافية للدمج.',code:`# ترتيب الدمج — Merge Sort — O(n log n)
دالة ادمج(يسار، يمين)
    متغير نتيجة = []
    متغير ت = 0
    متغير ج = 0
    بينما (ت < يسار.طول() و ج < يمين.طول())
        إذا (يسار[ت] <= يمين[ج])
            نتيجة.أضف(يسار[ت])
            ت = ت + 1
        وإلا
            نتيجة.أضف(يمين[ج])
            ج = ج + 1
        نهاية
    نهاية
    بينما (ت < يسار.طول())
        نتيجة.أضف(يسار[ت])
        ت = ت + 1
    نهاية
    بينما (ج < يمين.طول())
        نتيجة.أضف(يمين[ج])
        ج = ج + 1
    نهاية
    ارجع نتيجة
نهاية

دالة ترتيب_دمج(مصفوفة)
    إذا (مصفوفة.طول() <= 1)
        ارجع مصفوفة
    نهاية
    متغير وسط = مصفوفة.طول() / 2
    متغير يسار = مصفوفة.شريحة(0، وسط)
    متغير يمين = مصفوفة.شريحة(وسط)
    ارجع ادمج(ترتيب_دمج(يسار)، ترتيب_دمج(يمين))
نهاية

متغير أرقام = [38، 27، 43، 3، 9، 82، 10]
اطبع("قبل: " + أرقام)
متغير مرتبة = ترتيب_دمج(أرقام)
اطبع("بعد: " + مرتبة)`,exercise:"استخدم ترتيب الدمج لدمج قائمتين مرتبتين مسبقاً في قائمة واحدة.",hint:"دالة ادمج() تعمل مباشرة على قائمتين مرتبتين — استدعها بدون التقسيم.",tips:["ترتيب الدمج يقسم المشكلة إلى أنصاف","أداؤه مستقر O(n log n) في كل الحالات","يحتاج ذاكرة إضافية لمصفوفة النتيجة"],concepts:["ترتيب الدمج","Merge Sort","فرّق تسد","O(n log n)"]},{id:"l4_22",title:"البحث الخطي والثنائي",desc:"البحث في البيانات: التسلسلي البسيط مقابل الثنائي السريع.",explanation:"البحث الخطي كالبحث عن كلمة بقراءة كل القاموس، بينما الثنائي يفتح المنتصف ويقرر: أمام أم خلف؟ الفرق هائل: في مليون عنصر، الخطي قد يحتاج مليون مقارنة، الثنائي 20 فقط! شرط الثنائي: البيانات يجب أن تكون مرتبة.",code:`# البحث الخطي — O(n)
دالة بحث_خطي(مصفوفة، هدف)
    لكل ت في [0..مصفوفة.طول() - 1]
        إذا (مصفوفة[ت] == هدف)
            ارجع ت
        نهاية
    نهاية
    ارجع -1
نهاية

# البحث الثنائي — O(log n) — يتطلب مصفوفة مرتبة
دالة بحث_ثنائي(مصفوفة، هدف)
    متغير بداية = 0
    متغير نهاية = مصفوفة.طول() - 1
    بينما (بداية <= نهاية)
        متغير وسط = (بداية + نهاية) / 2
        إذا (مصفوفة[وسط] == هدف)
            ارجع وسط
        وإلا
            إذا (مصفوفة[وسط] < هدف)
                بداية = وسط + 1
            وإلا
                نهاية = وسط - 1
            نهاية
        نهاية
    نهاية
    ارجع -1
نهاية

# مقارنة
متغير بيانات = [2، 5، 8، 12، 16، 23، 38، 56، 72، 91]
اطبع("خطي — موقع 23: " + بحث_خطي(بيانات، 23))
اطبع("ثنائي — موقع 23: " + بحث_ثنائي(بيانات، 23))
اطبع("ثنائي — موقع 50: " + بحث_ثنائي(بيانات، 50))`,exercise:"أنشئ بحثاً ثنائياً بالعودية (recursive) بدلاً من الحلقة.",hint:"أنشئ دالة تأخذ بداية ونهاية ووسط وتستدعي نفسها.",tips:["البحث الخطي يعمل على أي مصفوفة","البحث الثنائي أسرع بكثير لكن يتطلب ترتيباً مسبقاً","البحث الثنائي O(log n) — يقسم المصفوفة للنصف كل مرة"],concepts:["بحث خطي","بحث ثنائي","Linear Search","Binary Search"]},{id:"l4_23",title:"أنماط العودية (Recursion)",desc:"حل مشكلات معقدة بتقسيمها إلى مشكلات أصغر متشابهة.",explanation:'العودية كمرآتين متقابلتين تعكس صورة لا نهائية — الدالة تستدعي نفسها. الفرق: لابد من "حالة الأساس" التي توقف الاستدعاء. مشاكل كثيرة أسهل عودياً: الأشجار، فيبوناتشي، التنقل في بنيات متداخلة. التذكر (Memoization) يتجنب تكرار الحسابات.',code:`# أنماط العودية المتقدمة

# 1. فيبوناتشي مع تذكر (Memoization)
دالة فيبوناتشي(ن، ذاكرة)
    إذا (ذاكرة[ن] != لاشيء)
        ارجع ذاكرة[ن]
    نهاية
    إذا (ن <= 1)
        ارجع ن
    نهاية
    ذاكرة[ن] = فيبوناتشي(ن - 1، ذاكرة) + فيبوناتشي(ن - 2، ذاكرة)
    ارجع ذاكرة[ن]
نهاية

اطبع("فيب(10): " + فيبوناتشي(10، {}))

# 2. أبراج هانوي
دالة هانوي(ن، من، إلى، وسيط)
    إذا (ن == 1)
        اطبع("انقل قرص من " + من + " إلى " + إلى)
        ارجع 1
    نهاية
    هانوي(ن - 1، من، وسيط، إلى)
    اطبع("انقل قرص من " + من + " إلى " + إلى)
    هانوي(ن - 1، وسيط، إلى، من)
نهاية

اطبع("--- أبراج هانوي (3 أقراص) ---")
هانوي(3، "أ"، "ج"، "ب")

# 3. تسطيح مصفوفة متداخلة
دالة سطّح(مصفوفة)
    متغير نتيجة = []
    لكل عنصر في مصفوفة
        إذا (عنصر == مصفوفة)
            # حماية من التداخل اللانهائي
            نتيجة.أضف(عنصر)
        وإلا
            لكل ف في عنصر
                نتيجة.أضف(ف)
            نهاية
        نهاية
    نهاية
    ارجع نتيجة
نهاية`,exercise:"اكتب دالة عودية لحساب أقصى قيمة في مصفوفة دون استخدام حلقات.",hint:"قارن العنصر الأول مع أقصى قيمة في بقية المصفوفة.",tips:["التذكر (Memoization) يتجنب الحسابات المكررة","حالة الأساس شرط أساسي لأي دالة عودية","تأكد من اقتراب كل استدعاء من حالة الأساس"],concepts:["عودية","Recursion","تذكر","Memoization"]},{id:"l4_24",title:"البرمجة الديناميكية",desc:"حل مشكلات التحسين بتقسيمها ثم حفظ النتائج الجزئية.",explanation:"البرمجة الديناميكية = عودية + ذاكرة. بدلاً من حساب نفس الشيء مراراً، احفظه وأعد استخدامه. مثال: لحساب فيب(10) تحتاج فيب(9) وفيب(8)، وكلاهما يحتاج فيب(7) — بدون حفظ ستحسبه مرات! أساسية في مسائل التحسين كحقيبة الظهر وأقصر مسار.",code:`# البرمجة الديناميكية — حل أمثل بحفظ النتائج

# مشكلة حقيبة الظهر (Knapsack) المبسطة
دالة حقيبة_ظهر(أوزان، قيم، سعة)
    متغير ن = أوزان.طول()
    # إنشاء جدول DP
    متغير جدول = []
    لكل ت في [0..ن]
        متغير صف = []
        لكل و في [0..سعة]
            صف.أضف(0)
        نهاية
        جدول.أضف(صف)
    نهاية

    # ملء الجدول
    لكل ت في [1..ن]
        لكل و في [1..سعة]
            إذا (أوزان[ت - 1] <= و)
                متغير مع = قيم[ت - 1] + جدول[ت - 1][و - أوزان[ت - 1]]
                متغير بدون = جدول[ت - 1][و]
                إذا (مع > بدون)
                    جدول[ت][و] = مع
                وإلا
                    جدول[ت][و] = بدون
                نهاية
            وإلا
                جدول[ت][و] = جدول[ت - 1][و]
            نهاية
        نهاية
    نهاية

    ارجع جدول[ن][سعة]
نهاية

متغير أوزان = [2، 3، 4، 5]
متغير قيم = [3، 4، 5، 6]
متغير سعة = 8
اطبع("أقصى قيمة: " + حقيبة_ظهر(أوزان، قيم، سعة))

# مشكلة تبديل العملات
دالة أقل_عملات(عملات، مبلغ)
    متغير جدول = []
    لكل ت في [0..مبلغ]
        جدول.أضف(مبلغ + 1)
    نهاية
    جدول[0] = 0
    لكل ت في [1..مبلغ]
        لكل عملة في عملات
            إذا (عملة <= ت)
                إذا (جدول[ت - عملة] + 1 < جدول[ت])
                    جدول[ت] = جدول[ت - عملة] + 1
                نهاية
            نهاية
        نهاية
    نهاية
    إذا (جدول[مبلغ] > مبلغ)
        ارجع -1
    نهاية
    ارجع جدول[مبلغ]
نهاية

اطبع("أقل عملات لـ 11: " + أقل_عملات([1، 5، 10]، 11))`,exercise:"اكتب حلاً بالبرمجة الديناميكية لحساب أطول سلسلة فرعية مشتركة بين نصين.",hint:"أنشئ جدول ثنائي الأبعاد وقارن الحروف — إذا تطابقا أضف 1 من القطر.",tips:["البرمجة الديناميكية = عودية + حفظ نتائج","تحدد البنية الفرعية المثلى والتداخل","جدول DP يحفظ النتائج لتجنب التكرار"],concepts:["برمجة ديناميكية","Dynamic Programming","حقيبة الظهر","Knapsack"]},{id:"l4_25",title:"البحث بالعرض أولاً (BFS)",desc:"استكشاف الرسم البياني طبقة بطبقة — أقصر مسار.",explanation:"فكر في موجات المياه عند رمي حجر — تنتشر دائرياً من المركز. BFS يزور جميع الجيران أولاً قبل الانتقال للمستوى التالي. هذا يضمن إيجاد أقصر مسار في رسم بياني غير موزون. أساس خرائط جوجل واقتراحات الأصدقاء.",code:`# البحث بالعرض أولاً — BFS
صنف رسم
    باني()
        هذا.عقد = {}
    نهاية
    دالة أضف_حافة(من، إلى)
        إذا (هذا.عقد[من] == لاشيء)
            هذا.عقد[من] = []
        نهاية
        إذا (هذا.عقد[إلى] == لاشيء)
            هذا.عقد[إلى] = []
        نهاية
        هذا.عقد[من].أضف(إلى)
        هذا.عقد[إلى].أضف(من)
    نهاية
نهاية

دالة بحث_عرض(رسم، بداية، هدف)
    متغير رتل = [بداية]
    متغير زيارة = {}
    متغير أب = {}
    زيارة[بداية] = صحيح
    أب[بداية] = لاشيء

    بينما (رتل.طول() > 0)
        متغير حالي = رتل[0]
        رتل = رتل.شريحة(1)

        إذا (حالي == هدف)
            # بناء المسار
            متغير مسار = []
            متغير ع = هدف
            بينما (ع != لاشيء)
                مسار.أضف(ع)
                ع = أب[ع]
            نهاية
            ارجع مسار.قلب()
        نهاية

        لكل جار في رسم.عقد[حالي]
            إذا (زيارة[جار] != صحيح)
                زيارة[جار] = صحيح
                أب[جار] = حالي
                رتل.أضف(جار)
            نهاية
        نهاية
    نهاية
    ارجع []
نهاية

متغير خريطة = جديد رسم()
خريطة.أضف_حافة("أ"، "ب")
خريطة.أضف_حافة("أ"، "ج")
خريطة.أضف_حافة("ب"، "د")
خريطة.أضف_حافة("ج"، "د")
خريطة.أضف_حافة("د"، "هـ")

متغير مسار = بحث_عرض(خريطة، "أ"، "هـ")
اطبع("أقصر مسار: " + مسار)`,exercise:"استخدم BFS لحل متاهة بسيطة ممثلة كمصفوفة ثنائية الأبعاد.",hint:"حوّل المتاهة إلى رسم بياني — كل خلية مفتوحة عقدة، وكل ممر حافة.",tips:["BFS يجد أقصر مسار في رسم بياني غير مُوزَّن","يستخدم رتل (FIFO) لاستكشاف طبقة بطبقة","خريطة الأب تتيح إعادة بناء المسار"],concepts:["BFS","بحث بالعرض","أقصر مسار","رسم بياني"]},{id:"l4_26",title:"البحث بالعمق أولاً (DFS)",desc:"استكشاف عميق للرسم البياني — التعمق قبل التوسع.",explanation:"DFS كالسير في متاهة بيد على الجدار — تمشي للنهاية ثم ترجع وتجرب طريقاً آخر. بعكس BFS، يتعمق لأقصى حد قبل التراجع. مفيد جداً لكشف الدورات، الترتيب الطوبولوجي، وحل المتاهات. ينفذ طبيعياً بالعودية أو بالمكدس.",code:`# البحث بالعمق أولاً — DFS
دالة بحث_عمق(رسم، بداية، زيارة)
    إذا (زيارة[بداية] == صحيح)
        ارجع لاشيء
    نهاية
    زيارة[بداية] = صحيح
    اطبع("زيارة: " + بداية)

    إذا (رسم[بداية] != لاشيء)
        لكل جار في رسم[بداية]
            بحث_عمق(رسم، جار، زيارة)
        نهاية
    نهاية
نهاية

# كشف الدورات في الرسم البياني
دالة يحتوي_دورة(رسم، عقدة، زيارة، أب)
    زيارة[عقدة] = صحيح
    إذا (رسم[عقدة] != لاشيء)
        لكل جار في رسم[عقدة]
            إذا (زيارة[جار] != صحيح)
                إذا (يحتوي_دورة(رسم، جار، زيارة، عقدة))
                    ارجع صحيح
                نهاية
            وإلا
                إذا (جار != أب)
                    ارجع صحيح
                نهاية
            نهاية
        نهاية
    نهاية
    ارجع خطأ
نهاية

# رسم بياني
متغير رسم_بياني = {
    "أ": ["ب"، "ج"]،
    "ب": ["أ"، "د"]،
    "ج": ["أ"، "د"]،
    "د": ["ب"، "ج"]
}

اطبع("--- DFS ---")
بحث_عمق(رسم_بياني، "أ"، {})
اطبع("يحتوي دورة: " + يحتوي_دورة(رسم_بياني، "أ"، {}، لاشيء))`,exercise:"استخدم DFS لحساب عدد المكونات المتصلة في رسم بياني.",hint:"شغّل DFS من كل عقدة غير مزارة — كل تشغيل = مكون متصل.",tips:["DFS يتعمق كحد أقصى قبل التراجع","يمكن تنفيذه بالعودية أو بالمكدس","مفيد لكشف الدورات والمكونات المتصلة"],concepts:["DFS","بحث بالعمق","دورة","مكون متصل"]},{id:"l4_27",title:"تحليل التعقيد الزمني",desc:"فهم وقياس كفاءة الخوارزميات باستخدام O الكبير.",explanation:"O الكبير يجيب: كيف ينمو الوقت مع نمو البيانات؟ O(1) = ثابت كفتح صفحة برقمها. O(n) = خطي كقراءة كل الكتاب. O(n²) = مصافحة كل شخص لكل شخص. O(log n) = البحث الثنائي — الأفضل. فهم Big O يجعلك تختار الخوارزمية المناسبة لحجم بياناتك.",code:`# تحليل التعقيد الزمني — Big O

# O(1) — ثابت
دالة أول_عنصر(مصفوفة)
    ارجع مصفوفة[0]
نهاية

# O(n) — خطي
دالة مجموع(مصفوفة)
    متغير ن = 0
    لكل عنصر في مصفوفة
        ن = ن + عنصر
    نهاية
    ارجع ن
نهاية

# O(n²) — تربيعي
دالة أزواج(مصفوفة)
    متغير عدد = 0
    لكل أ في مصفوفة
        لكل ب في مصفوفة
            عدد = عدد + 1
        نهاية
    نهاية
    ارجع عدد
نهاية

# قياس الأداء بالعدّ
دالة قس_أداء(اسم، دالة_اختبار، مصفوفة)
    متغير عمليات = دالة_اختبار(مصفوفة)
    اطبع(اسم + ": " + عمليات + " عملية لـ " + مصفوفة.طول() + " عنصر")
نهاية

# تطبيق: مقارنة البحث الخطي والثنائي
دالة عمليات_خطي(م)
    متغير عدد = 0
    لكل ع في م
        عدد = عدد + 1
    نهاية
    ارجع عدد
نهاية

دالة عمليات_ثنائي(م)
    متغير عدد = 0
    متغير ب = 0
    متغير ن = م.طول() - 1
    بينما (ب <= ن)
        عدد = عدد + 1
        متغير و = (ب + ن) / 2
        ب = و + 1
    نهاية
    ارجع عدد
نهاية

متغير بيانات = [1، 2، 3، 4، 5، 6، 7، 8، 9، 10]
قس_أداء("خطي"، عمليات_خطي، بيانات)
قس_أداء("ثنائي"، عمليات_ثنائي، بيانات)`,exercise:"أنشئ دالة تقارن عدد عمليات ثلاث خوارزميات ترتيب على مصفوفات بأحجام مختلفة.",hint:"شغّل كل خوارزمية على مصفوفات بحجم 10 و50 و100 وقارن العمليات.",tips:["O(1) < O(log n) < O(n) < O(n log n) < O(n²)","الحجم الكبير يكشف الفرق بين الخوارزميات","اختر الخوارزمية المناسبة لحجم بياناتك"],concepts:["تعقيد زمني","Big O","O(n)","O(n²)","O(log n)"]}]},bc={id:"l4_text_processing",title:"معالجة النصوص المتقدمة",icon:"📝",lessons:[{id:"l4_28",title:"محلل الرموز (Tokenizer)",desc:"تقسيم نص إلى وحدات ذات معنى — أساس كل المحللات.",explanation:"تخيل قراءة جملة: تقسمها لكلمات وعلامات قبل فهمها. التحليل المعجمي هو الخطوة الأولى في معالجة أي لغة — البرمجة أو الطبيعية. يحول النص الخام لقطع معروفة: أرقام، عوامل، كلمات مفتاحية. أساس بناء المترجمات والمفسرات.",code:`# محلل رموز — تقسيم نص إلى رموز
صنف رمز
    باني(نوع، قيمة)
        هذا.نوع = نوع
        هذا.قيمة = قيمة
    نهاية
    دالة وصف()
        ارجع هذا.نوع + ":" + هذا.قيمة
    نهاية
نهاية

دالة حلل_رموز(نص)
    متغير رموز = []
    متغير ت = 0
    متغير أحرف = نص.قسم("")
    بينما (ت < أحرف.طول())
        متغير حرف = أحرف[ت]
        # تجاهل المسافات
        إذا (حرف == " ")
            ت = ت + 1
            استمر
        نهاية
        # أرقام
        إذا (حرف >= "0" و حرف <= "9")
            متغير رقم = ""
            بينما (ت < أحرف.طول() و أحرف[ت] >= "0" و أحرف[ت] <= "9")
                رقم = رقم + أحرف[ت]
                ت = ت + 1
            نهاية
            رموز.أضف(جديد رمز("رقم"، رقم))
            استمر
        نهاية
        # عوامل
        إذا (حرف == "+" أو حرف == "-" أو حرف == "*" أو حرف == "/")
            رموز.أضف(جديد رمز("عامل"، حرف))
        وإلا
            إذا (حرف == "(" أو حرف == ")")
                رموز.أضف(جديد رمز("قوس"، حرف))
            نهاية
        نهاية
        ت = ت + 1
    نهاية
    ارجع رموز
نهاية

متغير نتيجة = حلل_رموز("(3 + 45) * 2")
لكل ر في نتيجة
    اطبع(ر.وصف())
نهاية`,exercise:"وسّع المحلل ليدعم النصوص بين علامات اقتباس ومعرّفات (أسماء متغيرات).",hint:"للنصوص: اقرأ حتى علامة الاقتباس التالية. للمعرّفات: اقرأ أحرف متتالية.",tips:["التحليل المعجمي أول خطوة في معالجة اللغات","كل رمز له نوع (رقم، عامل) وقيمة","المؤشر ت يتقدم حرفاً بحرف"],concepts:["تحليل معجمي","Tokenizer","رموز","Lexer"]},{id:"l4_29",title:"محلل التعبيرات الحسابية",desc:"تقييم تعبيرات حسابية مع أولويات العمليات والأقواس.",explanation:"كيف تعرف الآلة الحاسبة أن الضرب أولاً ثم الجمع؟ خوارزمية Shunting-yard تستخدم مكدسين: للأرقام وللعمليات. كل عملية لها أولوية، والأقواس تفرض أولوية خاصة. هذا المفهوم أساس لبناء حاسبات ومفسرات لغات.",code:`# محلل تعبيرات حسابية بالمكدس
دالة قيّم_تعبير(تعبير)
    متغير أرقام = []
    متغير عمليات = []
    متغير أحرف = تعبير.قسم("")
    متغير ت = 0

    بينما (ت < أحرف.طول())
        متغير ح = أحرف[ت]
        إذا (ح == " ")
            ت = ت + 1
            استمر
        نهاية
        إذا (ح >= "0" و ح <= "9")
            متغير رقم = ""
            بينما (ت < أحرف.طول() و أحرف[ت] >= "0" و أحرف[ت] <= "9")
                رقم = رقم + أحرف[ت]
                ت = ت + 1
            نهاية
            أرقام.أضف(رقم * 1)
            استمر
        نهاية
        إذا (ح == "(")
            عمليات.أضف(ح)
        وإلا
            إذا (ح == ")")
                بينما (عمليات[عمليات.طول() - 1] != "(")
                    أرقام = طبّق(أرقام، عمليات)
                نهاية
                عمليات = عمليات.شريحة(0، عمليات.طول() - 1)
            وإلا
                بينما (عمليات.طول() > 0 و أولوية(عمليات[عمليات.طول() - 1]) >= أولوية(ح))
                    أرقام = طبّق(أرقام، عمليات)
                نهاية
                عمليات.أضف(ح)
            نهاية
        نهاية
        ت = ت + 1
    نهاية
    بينما (عمليات.طول() > 0)
        أرقام = طبّق(أرقام، عمليات)
    نهاية
    ارجع أرقام[0]
نهاية

دالة أولوية(عامل)
    إذا (عامل == "+" أو عامل == "-")
        ارجع 1
    نهاية
    إذا (عامل == "*" أو عامل == "/")
        ارجع 2
    نهاية
    ارجع 0
نهاية

دالة طبّق(أرقام، عمليات)
    متغير ب = أرقام[أرقام.طول() - 1]
    أرقام = أرقام.شريحة(0، أرقام.طول() - 1)
    متغير أ = أرقام[أرقام.طول() - 1]
    أرقام = أرقام.شريحة(0، أرقام.طول() - 1)
    متغير ع = عمليات[عمليات.طول() - 1]
    عمليات = عمليات.شريحة(0، عمليات.طول() - 1)
    متغير نتيجة = 0
    طابق (ع)
        عندما "+": نتيجة = أ + ب
        عندما "-": نتيجة = أ - ب
        عندما "*": نتيجة = أ * ب
        عندما "/": نتيجة = أ / ب
    نهاية
    أرقام.أضف(نتيجة)
    ارجع أرقام
نهاية

اطبع("3 + 4 * 2 = " + قيّم_تعبير("3 + 4 * 2"))
اطبع("(3 + 4) * 2 = " + قيّم_تعبير("(3 + 4) * 2"))`,exercise:"أضف دعم الأُس (^) بأعلى أولوية والسالب (أرقام سالبة).",hint:'أضف حالة "^" في أولوية() بقيمة 3 وفي طبّق() بعملية **.',tips:["خوارزمية Shunting-yard تحترم الأولويات","مكدسان: للأرقام وللعمليات","الأقواس تفرض أولوية خاصة"],concepts:["محلل تعبيرات","أولويات","مكدس","Expression Parser"]},{id:"l4_30",title:"محرك القوالب (Template Engine)",desc:"بناء محرك قوالب بسيط يستبدل المتغيرات في نص.",explanation:'القوالب تفصل التصميم عن البيانات: "مرحباً {{name}}!" تصبح "مرحباً أحمد!" عند التنفيذ. المحرك يبحث عن العلامات {{...}} ويستبدلها بالقيم. هذا المبدأ أساس كل محركات القوالب من Jinja إلى Mustache إلى Vue templates.',code:`# محرك قوالب بسيط — استبدال {{متغير}}
صنف محرك_قوالب
    باني()
        هذا.متغيرات = {}
    نهاية

    دالة عيّن(اسم، قيمة)
        هذا.متغيرات[اسم] = قيمة
    نهاية

    دالة حلل(قالب)
        متغير نتيجة = ""
        متغير أحرف = قالب.قسم("")
        متغير ت = 0
        بينما (ت < أحرف.طول())
            إذا (ت + 1 < أحرف.طول() و أحرف[ت] == "{" و أحرف[ت + 1] == "{")
                # بداية متغير
                ت = ت + 2
                متغير اسم = ""
                بينما (ت + 1 < أحرف.طول())
                    إذا (أحرف[ت] == "}" و أحرف[ت + 1] == "}")
                        ت = ت + 2
                        توقف
                    نهاية
                    اسم = اسم + أحرف[ت]
                    ت = ت + 1
                نهاية
                اسم = اسم.قص()
                إذا (هذا.متغيرات[اسم] != لاشيء)
                    نتيجة = نتيجة + هذا.متغيرات[اسم]
                وإلا
                    نتيجة = نتيجة + "{{" + اسم + "}}"
                نهاية
            وإلا
                نتيجة = نتيجة + أحرف[ت]
                ت = ت + 1
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير م = جديد محرك_قوالب()
م.عيّن("اسم"، "أحمد")
م.عيّن("مدينة"، "الرياض")
م.عيّن("عمر"، "25")

متغير قالب = "مرحباً {{اسم}}! أنت من {{مدينة}} وعمرك {{عمر}} سنة."
اطبع(م.حلل(قالب))`,exercise:"أضف دعم الشروط البسيطة: {{إذا:متغير}}نص{{/إذا}} يظهر النص إذا المتغير موجود.",hint:"ابحث عن {{إذا:...}} وتحقق من وجود المتغير — إذا موجود أبقِ النص.",tips:["محركات القوالب تفصل البيانات عن العرض","التحليل حرفاً بحرف يتيح مرونة كبيرة","مبدأ مشابه لكل أنظمة القوالب"],concepts:["محرك قوالب","Template Engine","استبدال"]},{id:"l4_31",title:"محلل CSV",desc:"قراءة وتحليل بيانات CSV إلى هياكل بيانات مفيدة.",explanation:"CSV (قيم مفصولة بفواصل) الصيغة الأبسط لتبادل البيانات الجدولية. كل ملف Excel يمكن تصديره كـ CSV. التحليل: السطر الأول رؤوس الأعمدة، البقية سجلات. كل سجل يصبح خريطة مفتاح/قيمة. مهارة أساسية لأي موظف يتعامل مع بيانات.",code:`# محلل CSV — تحويل نصوص مفصولة لبيانات منظمة
صنف محلل_CSV
    باني(فاصل)
        هذا.فاصل = فاصل
    نهاية

    دالة حلل(نص)
        متغير أسطر = نص.قسم("\\n")
        إذا (أسطر.طول() == 0)
            ارجع []
        نهاية
        # السطر الأول = رؤوس الأعمدة
        متغير رؤوس = أسطر[0].قسم(هذا.فاصل)
        متغير نتيجة = []
        لكل ت في [1..أسطر.طول() - 1]
            متغير قيم = أسطر[ت].قسم(هذا.فاصل)
            متغير سجل = {}
            لكل ج في [0..رؤوس.طول() - 1]
                إذا (ج < قيم.طول())
                    سجل[رؤوس[ج].قص()] = قيم[ج].قص()
                نهاية
            نهاية
            نتيجة.أضف(سجل)
        نهاية
        ارجع نتيجة
    نهاية

    دالة إلى_نص(بيانات)
        إذا (بيانات.طول() == 0)
            ارجع ""
        نهاية
        متغير رؤوس = []
        لكل مفتاح في بيانات[0]
            رؤوس.أضف(مفتاح)
        نهاية
        متغير نتيجة = ""
        لكل ت في [0..رؤوس.طول() - 1]
            إذا (ت > 0)
                نتيجة = نتيجة + هذا.فاصل
            نهاية
            نتيجة = نتيجة + رؤوس[ت]
        نهاية
        لكل سجل في بيانات
            نتيجة = نتيجة + "\\n"
            لكل ت في [0..رؤوس.طول() - 1]
                إذا (ت > 0)
                    نتيجة = نتيجة + هذا.فاصل
                نهاية
                نتيجة = نتيجة + سجل[رؤوس[ت]]
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير بيانات_خام = "الاسم,العمر,المدينة\\nأحمد,25,الرياض\\nسارة,30,جدة"
متغير م = جديد محلل_CSV(",")
متغير بيانات = م.حلل(بيانات_خام)
لكل سجل في بيانات
    اطبع(سجل["الاسم"] + " — " + سجل["المدينة"])
نهاية`,exercise:'أضف دالة "ترشيح" تقبل دالة شرط وترجع السجلات المطابقة.',hint:"أنشئ دالة تأخذ البيانات ودالة فلتر وترجع العناصر التي تحقق الشرط.",tips:["CSV = Comma Separated Values","السطر الأول عادة رؤوس الأعمدة","كل سجل يصبح خريطة مفتاح/قيمة"],concepts:["CSV","تحليل بيانات","سجلات"]},{id:"l4_32",title:"بيانات شبيهة بـ JSON",desc:"بناء وتحليل بيانات متداخلة بصيغة خريطة/مصفوفة.",explanation:"البيانات المتداخلة تمثل الواقع: مستخدم له عنوان، والعنوان له مدينة وشارع. الخرائط داخل خرائط تحاكي JSON — الصيغة الأساسية لتبادل البيانات على الويب. الوصول بالمسار (user.address.city) يسهل التنقل في العمق.",code:`# بيانات متداخلة — شبيهة بـ JSON
دالة أنشئ_مستخدم(اسم، عمر، هوايات)
    ارجع {
        "اسم": اسم،
        "عمر": عمر،
        "هوايات": هوايات،
        "عنوان": {
            "مدينة": "الرياض"،
            "حي": "النخيل"
        }
    }
نهاية

# تحويل إلى نص JSON-like
دالة إلى_نص_JSON(كائن، مستوى)
    متغير مسافة = ""
    لكل ت في [0..مستوى - 1]
        مسافة = مسافة + "  "
    نهاية
    متغير نتيجة = "{\\n"
    متغير أول = صحيح
    لكل مفتاح في كائن
        إذا (!أول)
            نتيجة = نتيجة + "،\\n"
        نهاية
        أول = خطأ
        نتيجة = نتيجة + مسافة + "  \\"" + مفتاح + "\\": "
        متغير قيمة = كائن[مفتاح]
        طابق (قيمة)
            افتراضي:
                نتيجة = نتيجة + "\\"" + قيمة + "\\""
        نهاية
    نهاية
    نتيجة = نتيجة + "\\n" + مسافة + "}"
    ارجع نتيجة
نهاية

# استعلام عميق
دالة اقرأ_مسار(كائن، مسار)
    متغير أجزاء = مسار.قسم(".")
    متغير حالي = كائن
    لكل جزء في أجزاء
        إذا (حالي == لاشيء)
            ارجع لاشيء
        نهاية
        حالي = حالي[جزء]
    نهاية
    ارجع حالي
نهاية

متغير مستخدم = أنشئ_مستخدم("أحمد"، 25، ["قراءة"، "برمجة"])
اطبع("المدينة: " + اقرأ_مسار(مستخدم، "عنوان.مدينة"))
اطبع("أول هواية: " + مستخدم["هوايات"][0])`,exercise:'أنشئ دالة "عيّن_مسار" تعيّن قيمة في مسار عميق (مثل "عنوان.رمز_بريدي").',hint:"تنقل عبر الأجزاء وأنشئ خرائط فارغة إذا لم تكن موجودة.",tips:["الخرائط المتداخلة تحاكي بيانات JSON","الوصول بالمسار يسهل التنقل في البيانات العميقة","مفيد لتخزين الإعدادات والبيانات المعقدة"],concepts:["بيانات متداخلة","JSON","مسار عميق"]},{id:"l4_33",title:"البحث في النصوص",desc:"خوارزميات بحث في النصوص — مطابقة أنماط بسيطة.",explanation:"أبسط بحث: قارن النمط بكل موقع في النص O(n*m). للنصوص القصيرة كافية. خوارزميات متقدمة مثل KMP و Boyer-Moore أسرع للنصوص الطويلة. البحث النصي أساس محركات البحث، المحررات، وأدوات grep/find.",code:`# بحث في النصوص — مطابقة أنماط
دالة ابحث_كل(نص، نمط)
    متغير مواقع = []
    متغير ط_نص = نص.طول()
    متغير ط_نمط = نمط.طول()
    إذا (ط_نمط > ط_نص)
        ارجع مواقع
    نهاية
    لكل ت في [0..ط_نص - ط_نمط]
        متغير تطابق = صحيح
        لكل ج في [0..ط_نمط - 1]
            إذا (نص.جزء(ت + ج، ت + ج + 1) != نمط.جزء(ج، ج + 1))
                تطابق = خطأ
                توقف
            نهاية
        نهاية
        إذا (تطابق)
            مواقع.أضف(ت)
        نهاية
    نهاية
    ارجع مواقع
نهاية

# بحث مع تمييز النتائج
دالة ميّز(نص، نمط)
    متغير مواقع = ابحث_كل(نص، نمط)
    إذا (مواقع.طول() == 0)
        اطبع("لم يُوجد: " + نمط)
        ارجع نص
    نهاية
    اطبع("وُجد " + مواقع.طول() + " تطابق")
    # استبدال بتمييز
    ارجع نص.استبدل(نمط، "»" + نمط + "«")
نهاية

متغير نص = "البرمجة بلغة ص ممتعة والبرمجة مفيدة"
اطبع(ميّز(نص، "البرمجة"))
اطبع("مواقع: " + ابحث_كل(نص، "البرمجة"))`,exercise:"أنشئ بحثاً غير حساس لحالة الأحرف (كبيرة/صغيرة) في النصوص اللاتينية.",hint:"حوّل كلا النصين إلى حالة واحدة قبل البحث.",tips:["البحث البسيط O(n*m) كافٍ للنصوص القصيرة","خوارزميات متقدمة مثل KMP أسرع","تمييز النتائج يساعد في عرض البحث"],concepts:["بحث نصي","مطابقة أنماط","String Search"]},{id:"l4_34",title:"إحصائيات النصوص",desc:"تحليل نص وحساب إحصائيات مفصلة: كلمات، تكرارات، أنماط.",explanation:"تحليل النصوص يكشف أنماطاً خفية: الكلمات الأكثر تكراراً تكشف الموضوع، متوسط الكلمات يكشف الأسلوب. الخرائط مثالية لعد التكرارات. هذه التقنيات أساس محركات البحث، تحليل المشاعر، وكشف السرقة الأدبية.",code:`# إحصائيات نصوص متقدمة
صنف محلل_نص
    باني(نص)
        هذا.نص = نص
        هذا.كلمات = نص.قسم(" ")
    نهاية

    دالة عدد_الكلمات()
        ارجع هذا.كلمات.طول()
    نهاية

    دالة عدد_الأحرف()
        ارجع هذا.نص.طول()
    نهاية

    دالة تكرارات_الكلمات()
        متغير عدّ = {}
        لكل كلمة في هذا.كلمات
            إذا (عدّ[كلمة] == لاشيء)
                عدّ[كلمة] = 0
            نهاية
            عدّ[كلمة] = عدّ[كلمة] + 1
        نهاية
        ارجع عدّ
    نهاية

    دالة أكثر_تكراراً(ن)
        متغير عدّ = هذا.تكرارات_الكلمات()
        متغير أزواج = []
        لكل مفتاح في عدّ
            أزواج.أضف([مفتاح، عدّ[مفتاح]])
        نهاية
        # ترتيب تنازلي
        متغير حجم = أزواج.طول()
        لكل ت في [0..حجم - 1]
            لكل ج في [0..حجم - ت - 2]
                إذا (أزواج[ج][1] < أزواج[ج + 1][1])
                    متغير مؤقت = أزواج[ج]
                    أزواج[ج] = أزواج[ج + 1]
                    أزواج[ج + 1] = مؤقت
                نهاية
            نهاية
        نهاية
        ارجع أزواج.شريحة(0، ن)
    نهاية

    دالة متوسط_طول_الكلمات()
        متغير مجموع = 0
        لكل كلمة في هذا.كلمات
            مجموع = مجموع + كلمة.طول()
        نهاية
        ارجع مجموع / هذا.كلمات.طول()
    نهاية

    دالة تقرير()
        اطبع("═══ تقرير النص ═══")
        اطبع("كلمات: " + هذا.عدد_الكلمات())
        اطبع("أحرف: " + هذا.عدد_الأحرف())
        اطبع("متوسط طول الكلمة: " + هذا.متوسط_طول_الكلمات())
        اطبع("أكثر 3 تكراراً:")
        لكل زوج في هذا.أكثر_تكراراً(3)
            اطبع("  " + زوج[0] + ": " + زوج[1])
        نهاية
    نهاية
نهاية

متغير نص = "البرمجة فن والبرمجة علم والبرمجة ممتعة والعلم نور"
متغير م = جديد محلل_نص(نص)
م.تقرير()`,exercise:"أضف دالة لحساب درجة قابلية القراءة بناءً على طول الكلمات والجمل.",hint:"الكلمات الطويلة والجمل الطويلة = صعوبة أعلى.",tips:["تحليل النصوص مفيد في محركات البحث","التكرارات تكشف الموضوعات الرئيسية","الخرائط مثالية لعد التكرارات"],concepts:["إحصائيات نصية","تكرارات","تحليل"]},{id:"l4_35",title:"تنسيق النصوص",desc:"أدوات تنسيق متقدمة — جداول، محاذاة، التفاف.",explanation:"التنسيق يحول البيانات الخام لعرض قابل للقراءة. الجداول تحتاج: حساب أعرض الأعمدة، توسيط النص، رسم الحدود. التفاف النص يحافظ على الكلمات كاملة عند القطع. هذه الأدوات تُبنى مرة وتُستخدم في كل مشروع.",code:`# أدوات تنسيق النصوص
دالة كرر(نص، عدد)
    متغير نتيجة = ""
    لكل ت في [0..عدد - 1]
        نتيجة = نتيجة + نص
    نهاية
    ارجع نتيجة
نهاية

دالة وسّط(نص، عرض)
    متغير فراغ = عرض - نص.طول()
    إذا (فراغ <= 0)
        ارجع نص
    نهاية
    متغير يسار = فراغ / 2
    متغير يمين = فراغ - يسار
    ارجع كرر(" "، يسار) + نص + كرر(" "، يمين)
نهاية

# طباعة جدول منسق
دالة اطبع_جدول(رؤوس، صفوف)
    متغير أعرض = []
    لكل ت في [0..رؤوس.طول() - 1]
        متغير أ = رؤوس[ت].طول()
        لكل صف في صفوف
            إذا (صف[ت].طول() > أ)
                أ = صف[ت].طول()
            نهاية
        نهاية
        أعرض.أضف(أ + 2)
    نهاية

    # رأس الجدول
    متغير خط = "+"
    لكل ع في أعرض
        خط = خط + كرر("-"، ع) + "+"
    نهاية
    اطبع(خط)
    متغير سطر_رأس = "|"
    لكل ت في [0..رؤوس.طول() - 1]
        سطر_رأس = سطر_رأس + وسّط(رؤوس[ت]، أعرض[ت]) + "|"
    نهاية
    اطبع(سطر_رأس)
    اطبع(خط)
    # صفوف البيانات
    لكل صف في صفوف
        متغير سطر = "|"
        لكل ت في [0..صف.طول() - 1]
            سطر = سطر + وسّط(صف[ت]، أعرض[ت]) + "|"
        نهاية
        اطبع(سطر)
    نهاية
    اطبع(خط)
نهاية

اطبع_جدول(
    ["الاسم"، "العمر"، "المدينة"]،
    [
        ["أحمد"، "25"، "الرياض"]،
        ["سارة"، "30"، "جدة"]،
        ["محمد"، "28"، "الدمام"]
    ]
)`,exercise:"أنشئ دالة تلف نصاً طويلاً عند عرض محدد مع الحفاظ على الكلمات كاملة.",hint:"قسّم النص إلى كلمات وابدأ سطراً جديداً عندما يتجاوز العرض.",tips:["التنسيق يجعل الإخراج قابلاً للقراءة","حساب أعرض عمود يضمن المحاذاة","الأدوات المساعدة تُبنى مرة وتُستخدم كثيراً"],concepts:["تنسيق نصوص","جداول","محاذاة"]},{id:"l4_36",title:"مفسر لغة صغيرة",desc:"بناء مفسر للغة برمجة مصغرة — تجميع كل مهارات النصوص.",explanation:"بناء مفسر بسيط يجمع كل ما تعلمته: تحليل معجمي لتقسيم الكود، خرائط للمتغيرات، طابق للأوامر. حتى لغة بسيطة (DSL) مفيدة جداً: لغات الإعداد، لغات الاستعلام، لغات الأتمتة. فهم المفسر يعمق فهمك للغات البرمجة ذاتها.",code:`# مفسر لغة صغيرة — حاسبة بمتغيرات
صنف مفسر_صغير
    باني()
        هذا.متغيرات = {}
    نهاية

    دالة نفّذ(سطر)
        متغير أجزاء = سطر.قص().قسم(" ")
        إذا (أجزاء.طول() == 0)
            ارجع لاشيء
        نهاية
        متغير أمر = أجزاء[0]
        طابق (أمر)
            عندما "عرّف":
                # عرّف س 10
                هذا.متغيرات[أجزاء[1]] = أجزاء[2] * 1
            عندما "اطبع":
                # اطبع س
                متغير اسم = أجزاء[1]
                إذا (هذا.متغيرات[اسم] != لاشيء)
                    اطبع(هذا.متغيرات[اسم])
                وإلا
                    اطبع(اسم)
                نهاية
            عندما "احسب":
                # احسب س + ص
                متغير أ = هذا._قيمة(أجزاء[1])
                متغير عامل = أجزاء[2]
                متغير ب = هذا._قيمة(أجزاء[3])
                متغير نتيجة = 0
                طابق (عامل)
                    عندما "+": نتيجة = أ + ب
                    عندما "-": نتيجة = أ - ب
                    عندما "*": نتيجة = أ * ب
                    عندما "/": نتيجة = أ / ب
                نهاية
                إذا (أجزاء.طول() > 5 و أجزاء[4] == "في")
                    هذا.متغيرات[أجزاء[5]] = نتيجة
                نهاية
                اطبع("= " + نتيجة)
            افتراضي:
                اطبع("خطأ: أمر غير معروف: " + أمر)
        نهاية
    نهاية

    دالة _قيمة(اسم)
        إذا (هذا.متغيرات[اسم] != لاشيء)
            ارجع هذا.متغيرات[اسم]
        نهاية
        ارجع اسم * 1
    نهاية

    دالة شغّل(برنامج)
        متغير أسطر = برنامج.قسم("\\n")
        لكل سطر في أسطر
            إذا (سطر.قص().طول() > 0)
                هذا.نفّذ(سطر)
            نهاية
        نهاية
    نهاية
نهاية

متغير م = جديد مفسر_صغير()
م.شغّل("عرّف س 10\\nعرّف ص 20\\nاحسب س + ص في ن\\nاطبع ن")`,exercise:'أضف دعم شرط بسيط: "إذا متغير > قيمة افعل أمر" لمفسرك.',hint:'أضف حالة "إذا" في طابق — قارن قيمة المتغير وإذا تحقق الشرط نفّذ بقية السطر.',tips:["المفسر يمر بمراحل: تحليل → تنفيذ","المتغيرات تُخزن في خريطة","هذا الأسلوب أساس كل لغات البرمجة"],concepts:["مفسر","Interpreter","لغة مصغرة","DSL"]}]},yc={id:"l4_data_processing",title:"معالجة البيانات",icon:"📊",lessons:[{id:"l4_37",title:"سلاسل التحويل (Pipelines)",desc:"بناء سلاسل تحويل بيانات — تمرير الإخراج كمدخل للتالي.",explanation:"سلاسل التحويل كخط إنتاج مصنع: كل محطة تأخذ المنتج، تعدله، وتمرره للتالية. filter → map → reduce نمط قوي جداً: رشح المطلوب، حوله، اجمعه. كل خطوة مستقلة وقابلة للاختبار وإعادة الاستخدام في سلاسل أخرى.",code:`# سلسلة تحويل البيانات — Pipeline
دالة خط_أنابيب(بيانات، تحويلات)
    متغير نتيجة = بيانات
    لكل تحويل في تحويلات
        نتيجة = تحويل(نتيجة)
    نهاية
    ارجع نتيجة
نهاية

# تحويلات جاهزة
دالة رشّح(شرط)
    ارجع لامدا (بيانات)
        متغير نتيجة = []
        لكل عنصر في بيانات
            إذا (شرط(عنصر))
                نتيجة.أضف(عنصر)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

دالة حوّل(دالة_تحويل)
    ارجع لامدا (بيانات)
        متغير نتيجة = []
        لكل عنصر في بيانات
            نتيجة.أضف(دالة_تحويل(عنصر))
        نهاية
        ارجع نتيجة
    نهاية
نهاية

دالة اختزل(دالة_دمج، قيمة_أولية)
    ارجع لامدا (بيانات)
        متغير نتيجة = قيمة_أولية
        لكل عنصر في بيانات
            نتيجة = دالة_دمج(نتيجة، عنصر)
        نهاية
        ارجع نتيجة
    نهاية
نهاية

# سلسلة: رشّح الأعداد الزوجية → ضاعفها → اجمعها
متغير أرقام = [1، 2، 3، 4، 5، 6، 7، 8، 9، 10]
متغير نتيجة = خط_أنابيب(أرقام، [
    رشّح(لامدا (ن) ارجع ن % 2 == 0 نهاية)،
    حوّل(لامدا (ن) ارجع ن * 2 نهاية)،
    اختزل(لامدا (مج، ن) ارجع مج + ن نهاية، 0)
])
اطبع("النتيجة: " + نتيجة)  # 60`,exercise:"أنشئ سلسلة تحويل لقائمة طلاب: فلتر الناجحين → حساب المتوسط → تقرير.",hint:"أنشئ ثلاث دوال تحويل وطبقها بالترتيب عبر خط_أنابيب.",tips:["سلاسل التحويل تجعل معالجة البيانات واضحة","كل خطوة مستقلة وقابلة لإعادة الاستخدام","الإغلاقات (closures) تغلف المعاملات"],concepts:["Pipeline","سلسلة تحويل","رشّح","حوّل","اختزل"]},{id:"l4_38",title:"التجميع والتصنيف (Grouping)",desc:"تجميع البيانات حسب معيار — مثل GROUP BY في SQL.",explanation:"التجميع يحول قائمة مسطحة لمجموعات ذات معنى: الموظفون حسب القسم، الطلاب حسب التقدير، المبيعات حسب الشهر. بعد التجميع يمكنك حساب إحصائيات كل مجموعة. أساس تقارير الأعمال ولوحات المراقبة (Dashboards).",code:`# تجميع البيانات حسب معيار
دالة جمّع_حسب(بيانات، دالة_مفتاح)
    متغير مجموعات = {}
    لكل عنصر في بيانات
        متغير مفتاح = دالة_مفتاح(عنصر)
        إذا (مجموعات[مفتاح] == لاشيء)
            مجموعات[مفتاح] = []
        نهاية
        مجموعات[مفتاح].أضف(عنصر)
    نهاية
    ارجع مجموعات
نهاية

# بيانات الموظفين
متغير موظفون = [
    {"اسم": "أحمد"، "قسم": "هندسة"، "راتب": 8000}،
    {"اسم": "سارة"، "قسم": "تسويق"، "راتب": 7000}،
    {"اسم": "محمد"، "قسم": "هندسة"، "راتب": 9000}،
    {"اسم": "فاطمة"، "قسم": "تسويق"، "راتب": 7500}،
    {"اسم": "خالد"، "قسم": "هندسة"، "راتب": 8500}،
    {"اسم": "نورة"، "قسم": "إدارة"، "راتب": 10000}
]

# تجميع حسب القسم
متغير حسب_القسم = جمّع_حسب(موظفون، لامدا (م) ارجع م["قسم"] نهاية)
لكل قسم في حسب_القسم
    متغير مجموعة = حسب_القسم[قسم]
    متغير مجموع = 0
    لكل م في مجموعة
        مجموع = مجموع + م["راتب"]
    نهاية
    متغير متوسط = مجموع / مجموعة.طول()
    اطبع(قسم + ": " + مجموعة.طول() + " موظف، متوسط الراتب: " + متوسط)
نهاية`,exercise:"جمّع الطلاب حسب التقدير (ممتاز، جيد، مقبول) واحسب إحصائيات كل مجموعة.",hint:"أنشئ دالة مفتاح تحول الدرجة إلى تقدير ثم استخدم جمّع_حسب.",tips:["التجميع أساسي في تحليل البيانات","دالة المفتاح تحدد معيار التجميع","يمكن تجميع حسب أي خاصية أو حساب"],concepts:["تجميع","GROUP BY","تصنيف","إحصائيات"]},{id:"l4_39",title:"سلاسل الترشيح (Filter Chains)",desc:"تطبيق عدة مرشحات متتابعة على البيانات.",explanation:`سلاسل الترشيح تُقرأ كلغة طبيعية: ".where(price > 1000).where(rating > 4).sortBy('price').limit(10)" واضحة جداً. كل دالة ترجع "هذا" للسماح بالتسلسل. هذا النمط (Fluent Interface) شائع في كل مكتبات البيانات وORMs.`,code:`# سلاسل الترشيح — تطبيق شروط متعددة
صنف مرشح_بيانات
    باني(بيانات)
        هذا.بيانات = بيانات
        هذا.نتيجة = بيانات
    نهاية

    دالة أين(شرط)
        متغير مرشحة = []
        لكل عنصر في هذا.نتيجة
            إذا (شرط(عنصر))
                مرشحة.أضف(عنصر)
            نهاية
        نهاية
        هذا.نتيجة = مرشحة
        ارجع هذا
    نهاية

    دالة رتّب_حسب(حقل، تنازلي)
        متغير ن = هذا.نتيجة.طول()
        لكل ت في [0..ن - 1]
            لكل ج في [0..ن - ت - 2]
                متغير شرط = خطأ
                إذا (تنازلي)
                    شرط = هذا.نتيجة[ج][حقل] < هذا.نتيجة[ج + 1][حقل]
                وإلا
                    شرط = هذا.نتيجة[ج][حقل] > هذا.نتيجة[ج + 1][حقل]
                نهاية
                إذا (شرط)
                    متغير مؤقت = هذا.نتيجة[ج]
                    هذا.نتيجة[ج] = هذا.نتيجة[ج + 1]
                    هذا.نتيجة[ج + 1] = مؤقت
                نهاية
            نهاية
        نهاية
        ارجع هذا
    نهاية

    دالة حدد(عدد)
        هذا.نتيجة = هذا.نتيجة.شريحة(0، عدد)
        ارجع هذا
    نهاية

    دالة النتائج()
        ارجع هذا.نتيجة
    نهاية
نهاية

متغير منتجات = [
    {"اسم": "هاتف"، "سعر": 3000، "تقييم": 4.5}،
    {"اسم": "حاسوب"، "سعر": 5000، "تقييم": 4.8}،
    {"اسم": "سماعة"، "سعر": 500، "تقييم": 4.2}،
    {"اسم": "ساعة"، "سعر": 1500، "تقييم": 4.0}،
    {"اسم": "كاميرا"، "سعر": 4000، "تقييم": 4.7}
]

# سلسلة: سعر > 1000 + تقييم > 4.3 + رتب بالسعر + أول 2
متغير أفضل = جديد مرشح_بيانات(منتجات)
    .أين(لامدا (م) ارجع م["سعر"] > 1000 نهاية)
    .أين(لامدا (م) ارجع م["تقييم"] > 4.3 نهاية)
    .رتّب_حسب("سعر"، صحيح)
    .حدد(2)
    .النتائج()

لكل م في أفضل
    اطبع(م["اسم"] + " — " + م["سعر"] + " ريال")
نهاية`,exercise:'أضف دالة "اختر" تعيد فقط حقولاً محددة (مثل SELECT في SQL).',hint:"أنشئ مصفوفة جديدة بخرائط تحتوي فقط الحقول المطلوبة.",tips:["سلاسل الترشيح تقرأ بشكل طبيعي",'كل دالة ترجع "هذا" لتمكين التسلسل',"نمط شائع في أطر العمل ومكتبات البيانات"],concepts:["ترشيح متسلسل","Filter Chain","استعلام"]},{id:"l4_40",title:"التحقق من البيانات (Validation)",desc:"بناء نظام تحقق مرن بقواعد قابلة للتركيب.",explanation:'التحقق المبكر يمنع البيانات الفاسدة من الدخول. القواعد القابلة للتركيب (".required().min(18).max(100)") واضحة وقابلة لإعادة الاستخدام. اجمع كل الأخطاء بدلاً من التوقف عند الأول — تجربة مستخدم أفضل. نمط Builder مع Validation.',code:`# نظام تحقق من البيانات
صنف محقق
    باني()
        هذا.قواعد = {}
    نهاية

    دالة حقل(اسم)
        هذا.قواعد[اسم] = []
        هذا._حقل_حالي = اسم
        ارجع هذا
    نهاية

    دالة مطلوب()
        هذا.قواعد[هذا._حقل_حالي].أضف({
            "نوع": "مطلوب"،
            "رسالة": هذا._حقل_حالي + " مطلوب"
        })
        ارجع هذا
    نهاية

    دالة أدنى(قيمة)
        متغير حقل = هذا._حقل_حالي
        هذا.قواعد[حقل].أضف({
            "نوع": "أدنى"،
            "قيمة": قيمة،
            "رسالة": حقل + " يجب أن يكون >= " + قيمة
        })
        ارجع هذا
    نهاية

    دالة أقصى(قيمة)
        متغير حقل = هذا._حقل_حالي
        هذا.قواعد[حقل].أضف({
            "نوع": "أقصى"،
            "قيمة": قيمة،
            "رسالة": حقل + " يجب أن يكون <= " + قيمة
        })
        ارجع هذا
    نهاية

    دالة تحقق(بيانات)
        متغير أخطاء = []
        لكل حقل في هذا.قواعد
            لكل قاعدة في هذا.قواعد[حقل]
                طابق (قاعدة["نوع"])
                    عندما "مطلوب":
                        إذا (بيانات[حقل] == لاشيء أو بيانات[حقل] == "")
                            أخطاء.أضف(قاعدة["رسالة"])
                        نهاية
                    عندما "أدنى":
                        إذا (بيانات[حقل] != لاشيء و بيانات[حقل] < قاعدة["قيمة"])
                            أخطاء.أضف(قاعدة["رسالة"])
                        نهاية
                    عندما "أقصى":
                        إذا (بيانات[حقل] != لاشيء و بيانات[حقل] > قاعدة["قيمة"])
                            أخطاء.أضف(قاعدة["رسالة"])
                        نهاية
                نهاية
            نهاية
        نهاية
        ارجع أخطاء
    نهاية
نهاية

متغير م = جديد محقق()
م.حقل("الاسم").مطلوب()
م.حقل("العمر").مطلوب().أدنى(18).أقصى(100)
م.حقل("الراتب").أدنى(0)

متغير أخطاء = م.تحقق({"الاسم": ""، "العمر": 15، "الراتب": 5000})
لكل خطأ في أخطاء
    اطبع("❌ " + خطأ)
نهاية`,exercise:'أضف قاعدة "نمط" للتحقق من أن النص يطابق شكلاً معيناً (مثل بريد إلكتروني بسيط).',hint:"تحقق من وجود @ وأن الطول مقبول كقاعدة بسيطة.",tips:["التحقق المبكر يمنع أخطاء وقت التشغيل","القواعد القابلة للتركيب مرنة وقابلة للتوسع","أنماط مشابهة مستخدمة في كل أطر العمل"],concepts:["تحقق","Validation","قواعد","Builder Pattern"]},{id:"l4_41",title:"الإحصائيات والتحليل",desc:"حساب مقاييس إحصائية: المتوسط، الوسيط، الانحراف المعياري.",explanation:"الإحصاء يلخص الفوضى بأرقام ذات معنى. المتوسط يتأثر بالقيم المتطرفة، الوسيط أكثر ثباتاً. التباين يقيس مدى تشتت البيانات — مفيد لكشف الشاذ ومراقبة الجودة. فهم الإحصاء يجعلك تقرأ البيانات بعمق.",code:`# مكتبة إحصائيات
صنف إحصاء
    باني(بيانات)
        هذا.بيانات = بيانات
        هذا._مرتبة = لاشيء
    نهاية

    دالة _رتب()
        إذا (هذا._مرتبة == لاشيء)
            هذا._مرتبة = []
            لكل ق في هذا.بيانات
                هذا._مرتبة.أضف(ق)
            نهاية
            هذا._مرتبة.رتب()
        نهاية
        ارجع هذا._مرتبة
    نهاية

    دالة المتوسط()
        متغير مجموع = 0
        لكل ق في هذا.بيانات
            مجموع = مجموع + ق
        نهاية
        ارجع مجموع / هذا.بيانات.طول()
    نهاية

    دالة الوسيط()
        متغير م = هذا._رتب()
        متغير ن = م.طول()
        إذا (ن % 2 == 0)
            ارجع (م[ن / 2 - 1] + م[ن / 2]) / 2
        نهاية
        ارجع م[ن / 2]
    نهاية

    دالة الأدنى()
        متغير أ = هذا.بيانات[0]
        لكل ق في هذا.بيانات
            إذا (ق < أ)
                أ = ق
            نهاية
        نهاية
        ارجع أ
    نهاية

    دالة الأقصى()
        متغير أ = هذا.بيانات[0]
        لكل ق في هذا.بيانات
            إذا (ق > أ)
                أ = ق
            نهاية
        نهاية
        ارجع أ
    نهاية

    دالة التباين()
        متغير م = هذا.المتوسط()
        متغير مجموع = 0
        لكل ق في هذا.بيانات
            مجموع = مجموع + (ق - م) ** 2
        نهاية
        ارجع مجموع / هذا.بيانات.طول()
    نهاية

    دالة تقرير()
        اطبع("═══ تقرير إحصائي ═══")
        اطبع("العدد: " + هذا.بيانات.طول())
        اطبع("المتوسط: " + هذا.المتوسط())
        اطبع("الوسيط: " + هذا.الوسيط())
        اطبع("الأدنى: " + هذا.الأدنى())
        اطبع("الأقصى: " + هذا.الأقصى())
        اطبع("التباين: " + هذا.التباين())
    نهاية
نهاية

متغير درجات = [85، 92، 78، 95، 88، 72، 90، 85، 93، 80]
متغير إ = جديد إحصاء(درجات)
إ.تقرير()`,exercise:"أضف دالة لحساب المدى الربعي (Q1, Q3) ودالة لكشف القيم الشاذة.",hint:"الربع الأول = وسيط النصف الأول. القيمة الشاذة خارج 1.5×IQR.",tips:["الإحصائيات أساسية في تحليل البيانات","التباين يقيس مدى تشتت البيانات","الترتيب المسبق يسرع حساب الوسيط"],concepts:["إحصاء","متوسط","وسيط","تباين"]},{id:"l4_42",title:"التقارير (Reporting)",desc:"إنشاء تقارير منسقة من بيانات خام.",explanation:'التقارير تلخص البيانات لمتخذي القرار: المدير لا يريد كل السجلات، يريد "أفضل 5 منتجات". نمط Builder يبني التقرير قسماً فقسماً. فصل البناء عن العرض يتيح تصدير نفس التقرير للشاشة أو PDF أو Excel.',code:`# نظام تقارير
صنف مولد_تقارير
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.أقسام = []
    نهاية

    دالة أضف_قسم(عنوان، محتوى)
        هذا.أقسام.أضف({"عنوان": عنوان، "محتوى": محتوى})
        ارجع هذا
    نهاية

    دالة أضف_جدول(عنوان، رؤوس، صفوف)
        هذا.أقسام.أضف({
            "عنوان": عنوان،
            "نوع": "جدول"،
            "رؤوس": رؤوس،
            "صفوف": صفوف
        })
        ارجع هذا
    نهاية

    دالة اطبع_تقرير()
        متغير خط = "══════════════════════════"
        اطبع(خط)
        اطبع("  " + هذا.عنوان)
        اطبع(خط)
        لكل قسم في هذا.أقسام
            اطبع("")
            اطبع("▶ " + قسم["عنوان"])
            اطبع("──────────────────")
            إذا (قسم["نوع"] == "جدول")
                لكل صف في قسم["صفوف"]
                    متغير سطر = "  "
                    لكل ت في [0..صف.طول() - 1]
                        سطر = سطر + قسم["رؤوس"][ت] + ": " + صف[ت] + "  "
                    نهاية
                    اطبع(سطر)
                نهاية
            وإلا
                اطبع("  " + قسم["محتوى"])
            نهاية
        نهاية
        اطبع(خط)
    نهاية
نهاية

# إنشاء تقرير مبيعات
متغير تقرير = جديد مولد_تقارير("تقرير المبيعات الشهري")
تقرير.أضف_قسم("ملخص"، "إجمالي المبيعات: 150,000 ريال")
    .أضف_جدول("أفضل المنتجات"،
        ["المنتج"، "المبيعات"]،
        [["هاتف"، "50000"]، ["حاسوب"، "40000"]])
    .أضف_قسم("ملاحظات"، "نمو 15% عن الشهر السابق")
    .اطبع_تقرير()`,exercise:"أضف دالة لتصدير التقرير كنص مسطح قابل للحفظ في ملف.",hint:"بدلاً من اطبع اجمع النصوص في متغير واحد وأرجعه.",tips:["التقارير تلخص البيانات للمستخدم النهائي","الباني المتسلسل يسهل بناء التقارير","فصل البناء عن العرض يتيح تنسيقات متعددة"],concepts:["تقارير","تنسيق","Builder"]},{id:"l4_43",title:"ETL: استخراج وتحويل وتحميل",desc:"نمط ETL — استخراج بيانات خام، تحويلها، وتحميلها.",explanation:"ETL أساس هندسة البيانات: استخرج من مصادر متعددة (APIs, CSV, قواعد بيانات)، حول (نظف، وحد، احسب)، حمل للهدف (مستودع بيانات، تقرير). كل مرحلة مستقلة وقابلة للاستبدال. مستخدم في Airflow وSpark وكل أدوات البيانات الكبرى.",code:`# ETL — Extract, Transform, Load
صنف خط_ETL
    باني(اسم)
        هذا.اسم = اسم
        هذا.مستخرج = لاشيء
        هذا.تحويلات = []
        هذا.محمّل = لاشيء
    نهاية

    دالة استخرج(دالة_استخراج)
        هذا.مستخرج = دالة_استخراج
        ارجع هذا
    نهاية

    دالة حوّل(دالة_تحويل)
        هذا.تحويلات.أضف(دالة_تحويل)
        ارجع هذا
    نهاية

    دالة حمّل(دالة_تحميل)
        هذا.محمّل = دالة_تحميل
        ارجع هذا
    نهاية

    دالة شغّل()
        اطبع("⚙️  بدء ETL: " + هذا.اسم)
        # استخراج
        متغير بيانات = هذا.مستخرج()
        اطبع("📥 استخراج: " + بيانات.طول() + " سجل")
        # تحويل
        لكل تحويل في هذا.تحويلات
            بيانات = تحويل(بيانات)
        نهاية
        اطبع("🔄 تحويل: " + بيانات.طول() + " سجل")
        # تحميل
        هذا.محمّل(بيانات)
        اطبع("✅ اكتمل ETL: " + هذا.اسم)
    نهاية
نهاية

# محاكاة ETL
متغير خط = جديد خط_ETL("بيانات الموظفين")
خط.استخرج(لامدا ()
    ارجع [
        {"اسم": "أحمد"، "راتب": "8000"، "قسم": "IT"}،
        {"اسم": "سارة"، "راتب": "7000"، "قسم": "HR"}،
        {"اسم": ""، "راتب": "5000"، "قسم": "IT"}
    ]
نهاية)
.حوّل(لامدا (بيانات)
    # إزالة السجلات غير المكتملة
    متغير نتيجة = []
    لكل س في بيانات
        إذا (س["اسم"] != "")
            نتيجة.أضف(س)
        نهاية
    نهاية
    ارجع نتيجة
نهاية)
.حوّل(لامدا (بيانات)
    # تحويل الراتب لرقم
    لكل س في بيانات
        س["راتب"] = س["راتب"] * 1
    نهاية
    ارجع بيانات
نهاية)
.حمّل(لامدا (بيانات)
    لكل س في بيانات
        اطبع("💾 حفظ: " + س["اسم"] + " - " + س["راتب"])
    نهاية
نهاية)
.شغّل()`,exercise:"أنشئ خط ETL يستخرج سجلات طلاب، يحسب تقديراتهم، ويصدر تقريراً.",hint:"أنشئ مرحلة استخراج للبيانات، تحويل للتقديرات، وتحميل لطباعة التقرير.",tips:["ETL نمط أساسي في هندسة البيانات","كل مرحلة مستقلة وقابلة للاستبدال","التحويلات المتسلسلة تنظف البيانات تدريجياً"],concepts:["ETL","استخراج","تحويل","تحميل"]},{id:"l4_44",title:"تطبيع البيانات (Normalization)",desc:"تحويل البيانات لأشكال قياسية — إزالة التكرار وتوحيد القيم.",explanation:'البيانات الحقيقية فوضوية: "الرياض" و"رياض" و"الرياض " كلها نفس المدينة لكن الحاسوب لا يعرف. التطبيع: قص المسافات، توحيد الصيغ، إزالة المكرر. "جودة البيانات" بقدر أهمية "كمية البيانات" — القمامة الداخلة = القمامة الخارجة.',code:`# تطبيع البيانات
دالة طبّع_نص(نص)
    ارجع نص.قص()
نهاية

دالة أزل_تكرارات(مصفوفة، مفتاح)
    متغير مرئي = {}
    متغير نتيجة = []
    لكل عنصر في مصفوفة
        متغير ق = عنصر[مفتاح]
        إذا (مرئي[ق] != صحيح)
            مرئي[ق] = صحيح
            نتيجة.أضف(عنصر)
        نهاية
    نهاية
    ارجع نتيجة
نهاية

دالة وحّد_حقل(بيانات، حقل، تعيينات)
    لكل سجل في بيانات
        متغير قيمة = سجل[حقل]
        إذا (تعيينات[قيمة] != لاشيء)
            سجل[حقل] = تعيينات[قيمة]
        نهاية
    نهاية
    ارجع بيانات
نهاية

# بيانات غير منظمة
متغير خام = [
    {"اسم": " أحمد "، "مدينة": "الرياض"، "هاتف": "0501234567"}،
    {"اسم": "سارة"، "مدينة": "رياض"، "هاتف": "0507654321"}،
    {"اسم": "أحمد"، "مدينة": "الرياض"، "هاتف": "0501234567"}،
    {"اسم": "محمد"، "مدينة": "جده"، "هاتف": "0509999999"}
]

# تطبيع: تنظيف + توحيد + إزالة تكرار
لكل سجل في خام
    سجل["اسم"] = طبّع_نص(سجل["اسم"])
نهاية
خام = وحّد_حقل(خام، "مدينة"، {"رياض": "الرياض"، "جده": "جدة"})
خام = أزل_تكرارات(خام، "هاتف")

اطبع("بعد التطبيع:")
لكل سجل في خام
    اطبع(سجل["اسم"] + " — " + سجل["مدينة"])
نهاية`,exercise:"أنشئ نظام تطبيع عناوين بريدية يوحد الاختصارات ويصحح الأخطاء الشائعة.",hint:'استخدم خريطة تعيينات للاختصارات مثل "ش" → "شارع".',tips:["تطبيع البيانات يضمن الاتساق","خرائط التعيين توحد القيم المختلفة","إزالة التكرار تحتاج مفتاحاً فريداً"],concepts:["تطبيع","Normalization","تنظيف بيانات"]},{id:"l4_45",title:"محرك إعداد بيانات شامل",desc:"تطبيق متكامل يجمع كل مهارات معالجة البيانات.",explanation:"هذا الدرس يجمع كل ما تعلمته في محرك واحد: رشح، حول، أضف حقول، اجمع، أنتج تقريراً. سجل العمليات يوثق خط الإعداد للمراجعة والتكرار. التسلسل الوظيفي يجعل الكود قابلاً للقراءة والصيانة.",code:`# محرك إعداد بيانات شامل — يجمع كل المفاهيم
صنف محرك_بيانات
    باني(بيانات)
        هذا.بيانات = بيانات
        هذا.سجل_عمليات = []
    نهاية

    دالة _سجّل(عملية)
        هذا.سجل_عمليات.أضف(عملية)
    نهاية

    دالة رشّح(شرط)
        متغير قبل = هذا.بيانات.طول()
        متغير نتيجة = []
        لكل ع في هذا.بيانات
            إذا (شرط(ع))
                نتيجة.أضف(ع)
            نهاية
        نهاية
        هذا.بيانات = نتيجة
        هذا._سجّل("ترشيح: " + قبل + " → " + نتيجة.طول())
        ارجع هذا
    نهاية

    دالة حوّل(حقل، دالة_تحويل)
        لكل ع في هذا.بيانات
            ع[حقل] = دالة_تحويل(ع[حقل])
        نهاية
        هذا._سجّل("تحويل حقل: " + حقل)
        ارجع هذا
    نهاية

    دالة أضف_حقل(اسم، دالة_حساب)
        لكل ع في هذا.بيانات
            ع[اسم] = دالة_حساب(ع)
        نهاية
        هذا._سجّل("إضافة حقل: " + اسم)
        ارجع هذا
    نهاية

    دالة ملخص()
        اطبع("═══ ملخص العمليات ═══")
        اطبع("عدد السجلات: " + هذا.بيانات.طول())
        لكل ع في هذا.سجل_عمليات
            اطبع("  • " + ع)
        نهاية
        ارجع هذا.بيانات
    نهاية
نهاية

متغير طلاب = [
    {"اسم": "أحمد"، "رياضيات": 85، "علوم": 90}،
    {"اسم": "سارة"، "رياضيات": 45، "علوم": 55}،
    {"اسم": "خالد"، "رياضيات": 92، "علوم": 88}
]

متغير نتائج = جديد محرك_بيانات(طلاب)
    .أضف_حقل("المعدل"، لامدا (ط) ارجع (ط["رياضيات"] + ط["علوم"]) / 2 نهاية)
    .أضف_حقل("التقدير"، لامدا (ط)
        إذا (ط["المعدل"] >= 85) ارجع "ممتاز" نهاية
        إذا (ط["المعدل"] >= 70) ارجع "جيد" نهاية
        ارجع "مقبول"
    نهاية)
    .رشّح(لامدا (ط) ارجع ط["المعدل"] >= 50 نهاية)
    .ملخص()

لكل ط في نتائج
    اطبع(ط["اسم"] + ": " + ط["التقدير"] + " (" + ط["المعدل"] + ")")
نهاية`,exercise:'أضف دالة "جمّع" للمحرك تجمع السجلات حسب حقل وتحسب إحصائيات كل مجموعة.',hint:"استخدم خريطة لتجميع السجلات ثم احسب المتوسط والعدد لكل مجموعة.",tips:["المحرك الشامل يجمع كل أدوات البيانات","سجل العمليات يوثق خط الإعداد","التسلسل الوظيفي يجعل الكود قابلاً للقراءة"],concepts:["محرك بيانات","ETL","ترشيح","تحويل","تجميع"]}]},$c={id:"l4_errors_reliability",title:"الأخطاء والموثوقية",icon:"🛡️",lessons:[{id:"l4_46",title:"أنواع أخطاء مخصصة",desc:"بناء تسلسل هرمي من أصناف الأخطاء لمعالجة دقيقة.",explanation:"الأخطاء المخصصة تحمل معلومات سياقية: خطأ تحقق يحمل اسم الحقل، خطأ شبكة يحمل رمز HTTP. التسلسل الهرمي يتيح معالجة عامة (كل أخطاء التطبيق) أو محددة (أخطاء الصلاحية فقط). مثل أي وراثة: المشترك في الأساس، الخاص في الفرعي.",code:`# أنواع أخطاء مخصصة
صنف خطأ_تطبيق
    باني(رسالة، رمز)
        هذا.رسالة = رسالة
        هذا.رمز = رمز
        هذا.النوع = "خطأ_تطبيق"
    نهاية
    دالة وصف()
        ارجع "[" + هذا.رمز + "] " + هذا.رسالة
    نهاية
نهاية

صنف خطأ_تحقق يرث خطأ_تطبيق
    باني(حقل، رسالة)
        الأساس(رسالة، "VALIDATION")
        هذا.حقل = حقل
        هذا.النوع = "خطأ_تحقق"
    نهاية
    دالة وصف()
        ارجع "❌ " + هذا.حقل + ": " + هذا.رسالة
    نهاية
نهاية

صنف خطأ_صلاحية يرث خطأ_تطبيق
    باني(عملية)
        الأساس("غير مصرح بـ: " + عملية، "AUTH")
        هذا.عملية = عملية
        هذا.النوع = "خطأ_صلاحية"
    نهاية
نهاية

صنف خطأ_غير_موجود يرث خطأ_تطبيق
    باني(مورد، معرّف)
        الأساس(مورد + " غير موجود: " + معرّف، "NOT_FOUND")
        هذا.المورد = مورد
        هذا.النوع = "خطأ_غير_موجود"
    نهاية
نهاية

# معالجة حسب النوع
دالة عالج_خطأ(خطأ)
    طابق (خطأ.النوع)
        عندما "خطأ_تحقق":
            اطبع("⚠️ خطأ بيانات: " + خطأ.وصف())
        عندما "خطأ_صلاحية":
            اطبع("🔒 رفض وصول: " + خطأ.وصف())
        عندما "خطأ_غير_موجود":
            اطبع("🔍 " + خطأ.وصف())
        افتراضي:
            اطبع("💥 خطأ عام: " + خطأ.وصف())
    نهاية
نهاية

حاول
    ارمي جديد خطأ_تحقق("البريد"، "صيغة غير صحيحة")
امسك (خ)
    عالج_خطأ(خ)
نهاية`,exercise:'أنشئ صنف "خطأ_شبكة" بمعلومات إضافية (رمز HTTP، عنوان URL) ومعالجة مناسبة.',hint:"وسّع خطأ_تطبيق وأضف خصائص للرمز والعنوان.",tips:["أنواع الأخطاء المخصصة تتيح معالجة دقيقة","التسلسل الهرمي يسمح بمعالجة عامة أو محددة","كل نوع يحمل معلومات سياقية مناسبة"],concepts:["أخطاء مخصصة","تسلسل هرمي","وراثة","طابق"]},{id:"l4_47",title:"البرمجة الدفاعية",desc:"كتابة كود يحمي نفسه من المدخلات غير المتوقعة.",explanation:"البرمجة الدفاعية تفترض الأسوأ: المستخدم سيدخل بيانات خاطئة، الشبكة ستفشل، الملف غير موجود. التأكيدات (assertions) تكشف الأخطاء مبكراً: القسمة على صفر تُكتشف قبل الحساب لا بعده. التحقق من المدخلات أول خط دفاعي.",code:`# البرمجة الدفاعية — توقع الأسوأ
دالة تأكد(شرط، رسالة)
    إذا (!شرط)
        ارمي "تأكيد فاشل: " + رسالة
    نهاية
نهاية

دالة تأكد_نوع(قيمة، نوع_متوقع، اسم)
    # فحص بسيط للنوع
    إذا (قيمة == لاشيء)
        ارمي اسم + " لا يمكن أن يكون لاشيء"
    نهاية
نهاية

# دالة آمنة للقسمة
دالة قسمة_آمنة(أ، ب)
    تأكد_نوع(أ، "رقم"، "المقسوم")
    تأكد_نوع(ب، "رقم"، "المقسوم عليه")
    تأكد(ب != 0، "لا يمكن القسمة على صفر")
    ارجع أ / ب
نهاية

# صنف آمن لإدارة المصفوفات
صنف مصفوفة_آمنة
    باني(بيانات)
        هذا.بيانات = بيانات
    نهاية

    دالة اقرأ(فهرس)
        تأكد(فهرس >= 0، "الفهرس لا يمكن أن يكون سالباً")
        تأكد(فهرس < هذا.بيانات.طول()، "الفهرس خارج النطاق")
        ارجع هذا.بيانات[فهرس]
    نهاية

    دالة عيّن(فهرس، قيمة)
        تأكد(فهرس >= 0 و فهرس < هذا.بيانات.طول()، "فهرس غير صالح")
        تأكد_نوع(قيمة، "أي"، "القيمة")
        هذا.بيانات[فهرس] = قيمة
    نهاية
نهاية

حاول
    اطبع(قسمة_آمنة(10، 3))
    اطبع(قسمة_آمنة(10، 0))
امسك (خ)
    اطبع("🛡️ " + خ)
نهاية`,exercise:'أنشئ صنف "خريطة_آمنة" يتحقق من المفاتيح ويمنع الكتابة فوق قيم موجودة بدون تأكيد.',hint:"أضف دالة تحقق من وجود المفتاح قبل الكتابة وارفض التكرار.",tips:["البرمجة الدفاعية تمنع أخطاء وقت التشغيل","التأكيدات (assertions) تكشف الأخطاء مبكراً","التحقق من المدخلات أول خط دفاع"],concepts:["برمجة دفاعية","تأكيدات","Defensive Programming"]},{id:"l4_48",title:"التحقق من المدخلات",desc:"بناء نظام شامل للتحقق من صحة مدخلات المستخدم.",explanation:"مدخلات المستخدم غير موثوقة: البريد دون @، العمر سالب، الاسم فارغ. نظام التحقق الجيد يجمع كل الأخطاء بدلاً من التوقف عند الأول — المستخدم يصحح كل شيء مرة واحدة. التحقق في الواجهة لتجربة مستخدم، وفي الخادم للأمان.",code:`# نظام تحقق شامل من المدخلات
صنف فاحص_مدخلات
    باني()
        هذا.أخطاء = []
    نهاية

    دالة نص_غير_فارغ(قيمة، اسم)
        إذا (قيمة == لاشيء أو قيمة == "")
            هذا.أخطاء.أضف(اسم + " لا يمكن أن يكون فارغاً")
            ارجع خطأ
        نهاية
        ارجع صحيح
    نهاية

    دالة رقم_في_نطاق(قيمة، أدنى، أقصى، اسم)
        إذا (قيمة < أدنى أو قيمة > أقصى)
            هذا.أخطاء.أضف(اسم + " يجب أن يكون بين " + أدنى + " و " + أقصى)
            ارجع خطأ
        نهاية
        ارجع صحيح
    نهاية

    دالة طول_نص(قيمة، أدنى، أقصى، اسم)
        إذا (قيمة.طول() < أدنى)
            هذا.أخطاء.أضف(اسم + " قصير جداً (الحد الأدنى " + أدنى + ")")
            ارجع خطأ
        نهاية
        إذا (قيمة.طول() > أقصى)
            هذا.أخطاء.أضف(اسم + " طويل جداً (الحد الأقصى " + أقصى + ")")
            ارجع خطأ
        نهاية
        ارجع صحيح
    نهاية

    دالة بريد_صالح(قيمة، اسم)
        إذا (!قيمة.يحتوي("@"))
            هذا.أخطاء.أضف(اسم + " يجب أن يحتوي @")
            ارجع خطأ
        نهاية
        ارجع صحيح
    نهاية

    دالة صالح()
        ارجع هذا.أخطاء.طول() == 0
    نهاية

    دالة اعرض_أخطاء()
        لكل خطأ في هذا.أخطاء
            اطبع("❌ " + خطأ)
        نهاية
    نهاية
نهاية

# تسجيل مستخدم
دالة سجّل_مستخدم(اسم، عمر، بريد، كلمة_سر)
    متغير ف = جديد فاحص_مدخلات()
    ف.نص_غير_فارغ(اسم، "الاسم")
    ف.طول_نص(اسم، 2، 50، "الاسم")
    ف.رقم_في_نطاق(عمر، 13، 120، "العمر")
    ف.بريد_صالح(بريد، "البريد")
    ف.طول_نص(كلمة_سر، 8، 100، "كلمة السر")

    إذا (!ف.صالح())
        ف.اعرض_أخطاء()
        ارجع خطأ
    نهاية
    اطبع("✅ تم التسجيل: " + اسم)
    ارجع صحيح
نهاية

سجّل_مستخدم("أ"، 10، "test"، "123")
سجّل_مستخدم("أحمد"، 25، "a@b.com"، "12345678")`,exercise:"أضف تحققاً من كلمة السر: يجب أن تحتوي أرقاماً وحروفاً.",hint:"امسح كل حرف وتحقق من وجود رقم واحد وحرف واحد على الأقل.",tips:["التحقق المبكر يمنع بيانات فاسدة","اجمع كل الأخطاء بدلاً من التوقف عند أول خطأ","قواعد التحقق قابلة لإعادة الاستخدام"],concepts:["تحقق مدخلات","Input Validation","نطاقات"]},{id:"l4_49",title:"نظام التسجيل (Logging)",desc:"بناء نظام تسجيل أحداث بمستويات مختلفة.",explanation:"التسجيل كصندوق أسود للطائرة: عند حدوث مشكلة، ارجع للسجل لتفهم ما حدث. المستويات (debug, info, warning, error) تتيح التحكم: في التطوير سجل كل شيء، في الإنتاج التحذيرات فأعلى فقط. ضروري لأي تطبيق إنتاجي.",code:`# نظام تسجيل أحداث (Logger)
صنف مسجل
    باني(اسم)
        هذا.اسم = اسم
        هذا.سجلات = []
        هذا.مستوى_أدنى = 0  # 0=تصحيح 1=معلومة 2=تحذير 3=خطأ
    نهاية

    دالة _سجّل(مستوى، رمز، رسالة)
        إذا (مستوى < هذا.مستوى_أدنى)
            ارجع لاشيء
        نهاية
        متغير سجل = {
            "مستوى": رمز،
            "مصدر": هذا.اسم،
            "رسالة": رسالة
        }
        هذا.سجلات.أضف(سجل)
        اطبع(رمز + " [" + هذا.اسم + "] " + رسالة)
    نهاية

    دالة تصحيح(رسالة)
        هذا._سجّل(0، "🔍"، رسالة)
    نهاية
    دالة معلومة(رسالة)
        هذا._سجّل(1، "ℹ️"، رسالة)
    نهاية
    دالة تحذير(رسالة)
        هذا._سجّل(2، "⚠️"، رسالة)
    نهاية
    دالة خطأ(رسالة)
        هذا._سجّل(3، "❌"، رسالة)
    نهاية

    دالة عيّن_مستوى(مستوى)
        هذا.مستوى_أدنى = مستوى
    نهاية

    دالة السجلات_حسب(مستوى)
        متغير نتيجة = []
        لكل س في هذا.سجلات
            إذا (س["مستوى"] == مستوى)
                نتيجة.أضف(س)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير سجل = جديد مسجل("التطبيق")
سجل.معلومة("بدء التطبيق")
سجل.تصحيح("تحميل الإعدادات")
سجل.تحذير("ذاكرة التخزين ممتلئة تقريباً")
سجل.خطأ("فشل الاتصال بقاعدة البيانات")
اطبع("عدد الأخطاء: " + سجل.السجلات_حسب("❌").طول())`,exercise:"أضف دالة لتصفية السجلات حسب فترة زمنية ودالة لتصدير السجلات كنص.",hint:"أضف طابع زمني لكل سجل واستخدمه في الترشيح.",tips:["التسجيل ضروري لتتبع المشكلات","المستويات تتيح التحكم في كمية التفاصيل","في الإنتاج: مستوى تحذير أو أعلى"],concepts:["تسجيل","Logger","مستويات","تصحيح"]},{id:"l4_50",title:"نمط إعادة المحاولة (Retry)",desc:"إعادة تنفيذ عملية فاشلة تلقائياً مع تأخير تصاعدي.",explanation:"الشبكات غير موثوقة: الطلب الفاشل قد ينجح بعد ثانية. إعادة المحاولة التلقائية تتعامل مع الفشل المؤقت. التأخير التصاعدي (1ث → 2ث → 4ث) يخفف الضغط عن الخادم المثقل. الحد الأقصى يمنع المحاولة إلى الأبد.",code:`# نمط إعادة المحاولة — Retry Pattern
دالة أعد_المحاولة(عملية، أقصى_محاولات، تأخير)
    متغير محاولة = 0
    بينما (محاولة < أقصى_محاولات)
        حاول
            متغير نتيجة = عملية()
            اطبع("✅ نجحت في المحاولة " + (محاولة + 1))
            ارجع نتيجة
        امسك (خ)
            محاولة = محاولة + 1
            إذا (محاولة < أقصى_محاولات)
                اطبع("⚠️ فشل (" + محاولة + "/" + أقصى_محاولات + "): " + خ)
                اطبع("   إعادة المحاولة...")
            وإلا
                اطبع("❌ فشلت جميع المحاولات: " + خ)
                ارمي "فشل بعد " + أقصى_محاولات + " محاولات: " + خ
            نهاية
        نهاية
    نهاية
نهاية

# محاكاة عملية غير مستقرة
متغير عدّاد = 0
دالة عملية_غير_مستقرة()
    عدّاد = عدّاد + 1
    إذا (عدّاد < 3)
        ارمي "الخادم مشغول"
    نهاية
    ارجع "بيانات من الخادم"
نهاية

حاول
    متغير نتيجة = أعد_المحاولة(عملية_غير_مستقرة، 5، 1000)
    اطبع("النتيجة: " + نتيجة)
امسك (خ)
    اطبع("فشل نهائي: " + خ)
نهاية`,exercise:"أضف تأخيراً تصاعدياً (exponential backoff) بين المحاولات.",hint:"اضرب التأخير في 2 بعد كل محاولة فاشلة.",tips:["إعادة المحاولة مهمة للعمليات غير الموثوقة","التأخير التصاعدي يخفف الضغط على الخادم","حدد عدداً أقصى لتجنب الحلقات اللانهائية"],concepts:["إعادة محاولة","Retry","Backoff"]},{id:"l4_51",title:"التدهور الرشيق (Graceful Degradation)",desc:"الاستمرار في العمل بوظائف مخفضة عند فشل جزء.",explanation:"فشل خدمة واحدة لا يجب أن يعطل التطبيق كله. إذا فشلت قاعدة البيانات، اقرأ من الكاش. إذا فشل البريد، احفظ للإرسال لاحقاً. البدائل المحددة مسبقاً تبقي التطبيق يعمل جزئياً بدلاً من التعطل الكامل.",code:`# التدهور الرشيق — استمرار العمل رغم الأخطاء
صنف خدمة
    باني(اسم، نشطة)
        هذا.اسم = اسم
        هذا.نشطة = نشطة
    نهاية
نهاية

صنف تطبيق_مرن
    باني()
        هذا.خدمات = {}
        هذا.بدائل = {}
    نهاية

    دالة سجّل_خدمة(اسم، خدمة، بديل)
        هذا.خدمات[اسم] = خدمة
        هذا.بدائل[اسم] = بديل
    نهاية

    دالة نفّذ(اسم_خدمة، عملية)
        حاول
            متغير خدمة = هذا.خدمات[اسم_خدمة]
            إذا (خدمة == لاشيء أو !خدمة.نشطة)
                ارمي "الخدمة غير متاحة"
            نهاية
            ارجع عملية(خدمة)
        امسك (خ)
            اطبع("⚠️ " + اسم_خدمة + " فشلت: " + خ)
            # تجربة البديل
            إذا (هذا.بدائل[اسم_خدمة] != لاشيء)
                اطبع("🔄 تبديل إلى البديل...")
                ارجع هذا.بدائل[اسم_خدمة]()
            نهاية
            ارجع لاشيء
        نهاية
    نهاية
نهاية

# تطبيق
متغير تطبيق = جديد تطبيق_مرن()
تطبيق.سجّل_خدمة("قاعدة_بيانات"،
    جديد خدمة("DB"، خطأ)،
    لامدا () اطبع("💾 قراءة من الذاكرة المؤقتة") ارجع "بيانات مؤقتة" نهاية)
تطبيق.سجّل_خدمة("بريد"،
    جديد خدمة("Email"، خطأ)،
    لامدا () اطبع("📝 حفظ للإرسال لاحقاً") ارجع "محفوظ" نهاية)

تطبيق.نفّذ("قاعدة_بيانات"، لامدا (خ) ارجع "بيانات" نهاية)
تطبيق.نفّذ("بريد"، لامدا (خ) ارجع "تم الإرسال" نهاية)`,exercise:"أضف مستوى ثالث: إذا فشل البديل أيضاً، سجّل الخطأ وأرجع قيمة افتراضية.",hint:"غلّف استدعاء البديل بحاول/امسك وأضف قيمة افتراضية.",tips:["التدهور الرشيق يبقي التطبيق يعمل جزئياً","البدائل المخزنة مسبقاً تؤمن استمرار الخدمة","أفضل من التعطل الكامل عند أي فشل"],concepts:["تدهور رشيق","Graceful Degradation","بدائل"]},{id:"l4_52",title:"قاطع الدائرة (Circuit Breaker)",desc:"حماية النظام بإيقاف الاستدعاءات المتكررة للخدمة الفاشلة.",explanation:"مثل قاطع الكهرباء: عند حمل زائد يقطع التيار لحماية المنزل. إذا فشلت خدمة 3 مرات، توقف عن استدعائها مؤقتاً. بعد فترة جرب طلب واحد (half-open)، إذا نجح افتح الدائرة. أساسي في الأنظمة الموزعة (Microservices).",code:`# قاطع الدائرة — Circuit Breaker
صنف قاطع_دائرة
    باني(حد_فشل، مدة_انتظار)
        هذا.حالة = "مغلق"  # مغلق = يعمل، مفتوح = محظور
        هذا.عدد_فشل = 0
        هذا.حد_فشل = حد_فشل
        هذا.مدة_انتظار = مدة_انتظار
        هذا.آخر_فشل = 0
        هذا.محاولة = 0
    نهاية

    دالة نفّذ(عملية)
        إذا (هذا.حالة == "مفتوح")
            هذا.محاولة = هذا.محاولة + 1
            إذا (هذا.محاولة > هذا.مدة_انتظار)
                اطبع("🔄 محاولة نصف مفتوح...")
                هذا.حالة = "نصف_مفتوح"
            وإلا
                اطبع("🚫 الدائرة مفتوحة — الطلب مرفوض")
                ارجع لاشيء
            نهاية
        نهاية

        حاول
            متغير نتيجة = عملية()
            هذا._نجاح()
            ارجع نتيجة
        امسك (خ)
            هذا._فشل()
            اطبع("⚠️ فشل: " + خ)
            ارجع لاشيء
        نهاية
    نهاية

    دالة _نجاح()
        هذا.عدد_فشل = 0
        هذا.حالة = "مغلق"
        هذا.محاولة = 0
    نهاية

    دالة _فشل()
        هذا.عدد_فشل = هذا.عدد_فشل + 1
        إذا (هذا.عدد_فشل >= هذا.حد_فشل)
            هذا.حالة = "مفتوح"
            هذا.محاولة = 0
            اطبع("🔴 الدائرة مفتوحة! حد الفشل تجاوز.")
        نهاية
    نهاية

    دالة الحالة()
        ارجع هذا.حالة + " (فشل: " + هذا.عدد_فشل + ")"
    نهاية
نهاية

# محاكاة
متغير قاطع = جديد قاطع_دائرة(3، 5)
متغير عدّاد = 0

لكل ت في [0..9]
    عدّاد = عدّاد + 1
    متغير نجح = عدّاد > 7
    قاطع.نفّذ(لامدا ()
        إذا (!نجح) ارمي "خطأ خادم" نهاية
        ارجع "بيانات"
    نهاية)
    اطبع("  الحالة: " + قاطع.الحالة())
نهاية`,exercise:"أضف إحصائيات للقاطع: عدد النجاحات والفشل ونسبة النجاح.",hint:"أضف عدادات لكل حالة واحسب النسبة في دالة إحصائيات().",tips:["قاطع الدائرة يحمي من الحمل الزائد","ثلاث حالات: مغلق، مفتوح، نصف مفتوح","مستخدم كثيراً في الأنظمة الموزعة"],concepts:["قاطع دائرة","Circuit Breaker","حماية"]},{id:"l4_53",title:"الاختبارات الذاتية",desc:"كتابة إطار اختبار بسيط والتحقق من صحة الكود.",explanation:"الاختبارات تكشف الأخطاء قبل المستخدم. كل اختبار يفحص حالة واحدة: assertEquals(جمع(2,3), 5). التأكيدات أنواع: يساوي، صحيح، يرمي خطأ. اجمع الاختبارات وشغلها بعد كل تعديل — شبكة أمان للكود.",code:`# إطار اختبار بسيط
متغير _نتائج = {"نجح": 0، "فشل": 0، "أخطاء": []}

دالة اختبر(اسم، دالة_اختبار)
    حاول
        دالة_اختبار()
        _نتائج["نجح"] = _نتائج["نجح"] + 1
        اطبع("  ✅ " + اسم)
    امسك (خ)
        _نتائج["فشل"] = _نتائج["فشل"] + 1
        _نتائج["أخطاء"].أضف(اسم + ": " + خ)
        اطبع("  ❌ " + اسم + " — " + خ)
    نهاية
نهاية

دالة تأكد_يساوي(فعلي، متوقع)
    إذا (فعلي != متوقع)
        ارمي "توقعت " + متوقع + " لكن حصلت على " + فعلي
    نهاية
نهاية

دالة تأكد_صحيح(قيمة)
    إذا (!قيمة) ارمي "توقعت صحيح" نهاية
نهاية

دالة تأكد_خطأ_يحدث(عملية)
    حاول
        عملية()
        ارمي "توقعت خطأ لكن لم يحدث"
    امسك (خ)
        # الخطأ متوقع — نجاح
    نهاية
نهاية

دالة تقرير_اختبارات()
    متغير مجموع = _نتائج["نجح"] + _نتائج["فشل"]
    اطبع("")
    اطبع("═══ نتائج الاختبارات ═══")
    اطبع("المجموع: " + مجموع + " | نجح: " + _نتائج["نجح"] + " | فشل: " + _نتائج["فشل"])
نهاية

# اختبار دالة جمع
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

اطبع("اختبارات دالة جمع:")
اختبر("جمع أرقام موجبة"، لامدا () تأكد_يساوي(جمع(2، 3)، 5) نهاية)
اختبر("جمع مع صفر"، لامدا () تأكد_يساوي(جمع(0، 5)، 5) نهاية)
اختبر("جمع أرقام سالبة"، لامدا () تأكد_يساوي(جمع(-1، -2)، -3) نهاية)
تقرير_اختبارات()`,exercise:'أضف دالة "تأكد_قريب" تتحقق من تقارب رقمين عشريين (مع هامش خطأ).',hint:"احسب الفرق المطلق وقارنه بهامش الخطأ.",tips:["الاختبارات تكشف الأخطاء مبكراً","كل اختبار يفحص حالة واحدة محددة","تأكد_خطأ_يحدث يتحقق من الأخطاء المتوقعة"],concepts:["اختبارات","Testing","تأكيدات","إطار اختبار"]},{id:"l4_54",title:"نظام موثوقية شامل",desc:"تطبيق متكامل يجمع كل أنماط الموثوقية.",explanation:"الموثوقية الحقيقية تجمع كل الطبقات: إعادة محاولة → قاطع دائرة → بديل → تسجيل. كل طبقة تضيف مرونة: الأولى تتعامل مع الفشل المؤقت، الثانية تحمي من الحمل الزائد، الثالثة توفر بيانات بديلة، الرابعة توثق كل شيء للتحليل.",code:`# نظام موثوقية شامل
صنف خدمة_موثوقة
    باني(اسم)
        هذا.اسم = اسم
        هذا.سجل = []
        هذا.حالات_فشل = 0
        هذا.حد_فشل = 3
        هذا.معطل = خطأ
    نهاية

    دالة _سجّل(مستوى، رسالة)
        هذا.سجل.أضف(مستوى + ": " + رسالة)
    نهاية

    دالة نفّذ(عملية، بديل)
        # فحص قاطع الدائرة
        إذا (هذا.معطل)
            هذا._سجّل("⚠️"، "الخدمة معطلة — استخدام البديل")
            إذا (بديل != لاشيء)
                ارجع بديل()
            نهاية
            ارجع لاشيء
        نهاية

        # محاولة مع إعادة المحاولة
        متغير محاولات = 3
        لكل ت في [0..محاولات - 1]
            حاول
                متغير نتيجة = عملية()
                هذا.حالات_فشل = 0
                هذا._سجّل("✅"، "نجاح")
                ارجع نتيجة
            امسك (خ)
                هذا.حالات_فشل = هذا.حالات_فشل + 1
                هذا._سجّل("❌"، "فشل #" + هذا.حالات_فشل + ": " + خ)
                إذا (هذا.حالات_فشل >= هذا.حد_فشل)
                    هذا.معطل = صحيح
                    هذا._سجّل("🔴"، "تم تعطيل الخدمة")
                نهاية
            نهاية
        نهاية

        # كل المحاولات فشلت — البديل
        إذا (بديل != لاشيء)
            ارجع بديل()
        نهاية
        ارجع لاشيء
    نهاية

    دالة حالة()
        إذا (هذا.معطل) ارجع "🔴 معطل" نهاية
        إذا (هذا.حالات_فشل > 0) ارجع "🟡 متدهور" نهاية
        ارجع "🟢 يعمل"
    نهاية
نهاية

متغير خدمة = جديد خدمة_موثوقة("API")
متغير عدّاد = 0

لكل ت في [0..4]
    عدّاد = عدّاد + 1
    خدمة.نفّذ(
        لامدا ()
            إذا (عدّاد < 4) ارمي "انتهت المهلة" نهاية
            ارجع "بيانات"
        نهاية،
        لامدا () ارجع "بيانات مخزنة مؤقتاً" نهاية
    )
    اطبع("الحالة: " + خدمة.حالة())
نهاية`,exercise:"أضف واجهة مراقبة تعرض حالة جميع الخدمات المسجلة وإحصائياتها.",hint:"أنشئ مدير خدمات يحفظ قائمة ويعرض حالة كل واحدة.",tips:["الموثوقية = إعادة محاولة + قاطع + بدائل + تسجيل","كل طبقة حماية تضيف مرونة للنظام","المراقبة تكشف المشاكل قبل تفاقمها"],concepts:["موثوقية","Reliability","قاطع دائرة","بدائل"]}]},Sc={id:"l4_applied_projects",title:"مشاريع تطبيقية",icon:"🏗️",lessons:[{id:"l4_55",title:"نظام محادثة (Chat)",desc:"بناء محاكي غرفة محادثة مع مستخدمين ورسائل.",explanation:"نظام المحادثة يجمع كل المفاهيم: أصناف (رسالة، غرفة، مستخدم)، مصفوفات (سجل الرسائل)، نمط المراقب (بث الرسائل). البحث في الرسائل يستخدم مهارات النصوص. مشروع واقعي يحاكي تطبيقات مثل Slack وWhatsApp.",code:`# نظام محادثة بسيط
صنف رسالة
    باني(مرسل، نص)
        هذا.مرسل = مرسل
        هذا.نص = نص
        هذا.رقم = 0
    نهاية
نهاية

صنف غرفة_محادثة
    باني(اسم)
        هذا.اسم = اسم
        هذا.أعضاء = []
        هذا.رسائل = []
        هذا.رقم_تالي = 1
    نهاية

    دالة انضم(مستخدم)
        هذا.أعضاء.أضف(مستخدم)
        هذا._بث("📢"، مستخدم + " انضم للغرفة")
    نهاية

    دالة أرسل(مرسل، نص)
        متغير ر = جديد رسالة(مرسل، نص)
        ر.رقم = هذا.رقم_تالي
        هذا.رقم_تالي = هذا.رقم_تالي + 1
        هذا.رسائل.أضف(ر)
        هذا._بث(مرسل، نص)
    نهاية

    دالة _بث(من، نص)
        اطبع("[" + هذا.اسم + "] " + من + ": " + نص)
    نهاية

    دالة آخر_رسائل(عدد)
        متغير بداية = هذا.رسائل.طول() - عدد
        إذا (بداية < 0) بداية = 0 نهاية
        ارجع هذا.رسائل.شريحة(بداية)
    نهاية

    دالة ابحث(كلمة)
        متغير نتائج = []
        لكل ر في هذا.رسائل
            إذا (ر.نص.يحتوي(كلمة))
                نتائج.أضف(ر)
            نهاية
        نهاية
        ارجع نتائج
    نهاية
نهاية

صنف خادم_محادثة
    باني()
        هذا.غرف = {}
    نهاية
    دالة أنشئ_غرفة(اسم)
        هذا.غرف[اسم] = جديد غرفة_محادثة(اسم)
        ارجع هذا.غرف[اسم]
    نهاية
نهاية

متغير خادم = جديد خادم_محادثة()
متغير عام = خادم.أنشئ_غرفة("عام")
عام.انضم("أحمد")
عام.انضم("سارة")
عام.أرسل("أحمد"، "مرحباً بالجميع!")
عام.أرسل("سارة"، "أهلاً أحمد")
عام.أرسل("أحمد"، "كيف حالكم؟")`,exercise:"أضف رسائل خاصة بين مستخدمين ونظام أوامر (/أعضاء، /بحث).",hint:'أنشئ دالة رسالة_خاصة وتحقق من أن النص يبدأ بـ "/" لتنفيذ الأوامر.',tips:["المحادثة تجمع: أصناف، مصفوفات، خرائط","نمط المراقب مناسب للبث","البحث في الرسائل يستخدم يحتوي()"],concepts:["محادثة","Chat","بث","بحث نصي"]},{id:"l4_56",title:"قاعدة بيانات مصغرة",desc:"بناء قاعدة بيانات في الذاكرة مع CRUD وفهارس.",explanation:"قاعدة البيانات في جوهرها: جداول تخزن سجلات، وعمليات CRUD (إنشاء، قراءة، تحديث، حذف). الاستعلامات باللامدا تشبه WHERE في SQL. فهم كيفية عملها داخلياً يجعلك مستخدماً أفضل للقواعد الحقيقية.",code:`# قاعدة بيانات مصغرة في الذاكرة
صنف جدول
    باني(اسم، حقول)
        هذا.اسم = اسم
        هذا.حقول = حقول
        هذا.سجلات = []
        هذا.رقم_تالي = 1
    نهاية

    دالة أدخل(سجل)
        سجل["_رقم"] = هذا.رقم_تالي
        هذا.رقم_تالي = هذا.رقم_تالي + 1
        هذا.سجلات.أضف(سجل)
        ارجع سجل["_رقم"]
    نهاية

    دالة اختر(شرط)
        متغير نتائج = []
        لكل سجل في هذا.سجلات
            إذا (شرط(سجل))
                نتائج.أضف(سجل)
            نهاية
        نهاية
        ارجع نتائج
    نهاية

    دالة حدّث(رقم، تغييرات)
        لكل سجل في هذا.سجلات
            إذا (سجل["_رقم"] == رقم)
                لكل مفتاح في تغييرات
                    سجل[مفتاح] = تغييرات[مفتاح]
                نهاية
                ارجع صحيح
            نهاية
        نهاية
        ارجع خطأ
    نهاية

    دالة احذف(رقم)
        متغير جديدة = []
        لكل سجل في هذا.سجلات
            إذا (سجل["_رقم"] != رقم)
                جديدة.أضف(سجل)
            نهاية
        نهاية
        هذا.سجلات = جديدة
    نهاية

    دالة عدد()
        ارجع هذا.سجلات.طول()
    نهاية
نهاية

صنف قاعدة_بيانات
    باني(اسم)
        هذا.اسم = اسم
        هذا.جداول = {}
    نهاية
    دالة أنشئ_جدول(اسم، حقول)
        هذا.جداول[اسم] = جديد جدول(اسم، حقول)
        ارجع هذا.جداول[اسم]
    نهاية
    دالة جدول(اسم)
        ارجع هذا.جداول[اسم]
    نهاية
نهاية

# استخدام
متغير قب = جديد قاعدة_بيانات("متجر")
متغير منتجات = قب.أنشئ_جدول("منتجات"، ["اسم"، "سعر"])
منتجات.أدخل({"اسم": "هاتف"، "سعر": 3000})
منتجات.أدخل({"اسم": "حاسوب"، "سعر": 5000})
منتجات.أدخل({"اسم": "سماعة"، "سعر": 200})

متغير غالي = منتجات.اختر(لامدا (س) ارجع س["سعر"] > 1000 نهاية)
لكل م في غالي
    اطبع(م["اسم"] + ": " + م["سعر"])
نهاية
اطبع("إجمالي المنتجات: " + منتجات.عدد())`,exercise:'أضف دالة "ضم" تربط بين جدولين بحقل مشترك (مثل JOIN).',hint:"لكل سجل في الأول ابحث في الثاني عن تطابق الحقل المشترك.",tips:["قواعد البيانات تنظم البيانات في جداول","CRUD: إنشاء، قراءة، تحديث، حذف","الفهارس تسرع البحث"],concepts:["قاعدة بيانات","CRUD","جداول","استعلام"]},{id:"l4_57",title:"محاكي خادم ويب",desc:"محاكاة خادم ويب بسيط مع توجيه الطلبات والاستجابات.",explanation:"خادم الويب في جوهره: ربط مسارات (/users، /products) بمعالجات. GET للقراءة، POST للإنشاء، PUT للتحديث، DELETE للحذف. رموز الاستجابة: 200 نجاح، 404 غير موجود، 500 خطأ خادم. أساس كل APIs الحديثة.",code:`# محاكي خادم ويب
صنف طلب_HTTP
    باني(طريقة، مسار، جسم)
        هذا.طريقة = طريقة
        هذا.مسار = مسار
        هذا.جسم = جسم
    نهاية
نهاية

صنف استجابة_HTTP
    باني(رمز، محتوى)
        هذا.رمز = رمز
        هذا.محتوى = محتوى
    نهاية
    دالة وصف()
        ارجع "HTTP " + هذا.رمز + " — " + هذا.محتوى
    نهاية
نهاية

صنف خادم_ويب
    باني()
        هذا.مسارات = []
    نهاية

    دالة سجّل(طريقة، مسار، معالج)
        هذا.مسارات.أضف({
            "طريقة": طريقة،
            "مسار": مسار،
            "معالج": معالج
        })
    نهاية

    دالة GET(مسار، معالج)
        هذا.سجّل("GET"، مسار، معالج)
    نهاية

    دالة POST(مسار، معالج)
        هذا.سجّل("POST"، مسار، معالج)
    نهاية

    دالة عالج(طلب)
        اطبع("→ " + طلب.طريقة + " " + طلب.مسار)
        لكل مسار في هذا.مسارات
            إذا (مسار["طريقة"] == طلب.طريقة و مسار["مسار"] == طلب.مسار)
                متغير استجابة = مسار["معالج"](طلب)
                اطبع("← " + استجابة.وصف())
                ارجع استجابة
            نهاية
        نهاية
        متغير ر = جديد استجابة_HTTP(404، "غير موجود")
        اطبع("← " + ر.وصف())
        ارجع ر
    نهاية
نهاية

# تطبيق
متغير خادم = جديد خادم_ويب()
خادم.GET("/"، لامدا (ط)
    ارجع جديد استجابة_HTTP(200، "مرحباً بالعالم!")
نهاية)
خادم.GET("/مستخدمين"، لامدا (ط)
    ارجع جديد استجابة_HTTP(200، "قائمة المستخدمين")
نهاية)
خادم.POST("/مستخدمين"، لامدا (ط)
    ارجع جديد استجابة_HTTP(201، "تم الإنشاء: " + ط.جسم)
نهاية)

خادم.عالج(جديد طلب_HTTP("GET"، "/"، ""))
خادم.عالج(جديد طلب_HTTP("GET"، "/مستخدمين"، ""))
خادم.عالج(جديد طلب_HTTP("POST"، "/مستخدمين"، "أحمد"))
خادم.عالج(جديد طلب_HTTP("GET"، "/غير_موجود"، ""))`,exercise:"أضف middleware لتسجيل الطلبات ومصادقة بسيطة.",hint:"أنشئ دوال وسيطة تُنفذ قبل المعالج وبعده.",tips:["الخادم يربط مسارات بمعالجات","HTTP يستخدم طرق: GET, POST, PUT, DELETE","رموز الاستجابة: 200 نجاح, 404 غير موجود"],concepts:["خادم ويب","HTTP","توجيه","REST"]},{id:"l4_58",title:"محاكي نظام ملفات",desc:"بناء شجرة ملفات ومجلدات مع التنقل والبحث.",explanation:"نظام الملفات شجرة: الجذر يحوي مجلدات، المجلدات تحوي ملفات ومجلدات. التنقل يحتاج تتبع المسار الحالي والأب. البحث العميق يستخدم DFS. فهم بنية نظام الملفات يساعدك في العمل على أي مشروع برمجي.",code:`# محاكي نظام ملفات
صنف ملف
    باني(اسم، محتوى)
        هذا.اسم = اسم
        هذا.محتوى = محتوى
        هذا.نوع = "ملف"
    نهاية
نهاية

صنف مجلد
    باني(اسم)
        هذا.اسم = اسم
        هذا.نوع = "مجلد"
        هذا.محتويات = {}
    نهاية

    دالة أضف(عنصر)
        هذا.محتويات[عنصر.اسم] = عنصر
    نهاية

    دالة اقرأ(اسم)
        ارجع هذا.محتويات[اسم]
    نهاية

    دالة قائمة()
        متغير نتيجة = []
        لكل اسم في هذا.محتويات
            متغير رمز = "📄"
            إذا (هذا.محتويات[اسم].نوع == "مجلد")
                رمز = "📁"
            نهاية
            نتيجة.أضف(رمز + " " + اسم)
        نهاية
        ارجع نتيجة
    نهاية
نهاية

صنف نظام_ملفات
    باني()
        هذا.الجذر = جديد مجلد("/")
        هذا.الحالي = هذا.الجذر
        هذا.المسار = "/"
    نهاية

    دالة أنشئ_ملف(اسم، محتوى)
        هذا.الحالي.أضف(جديد ملف(اسم، محتوى))
    نهاية

    دالة أنشئ_مجلد(اسم)
        هذا.الحالي.أضف(جديد مجلد(اسم))
    نهاية

    دالة ادخل(اسم)
        متغير عنصر = هذا.الحالي.اقرأ(اسم)
        إذا (عنصر != لاشيء و عنصر.نوع == "مجلد")
            هذا.الحالي = عنصر
            هذا.المسار = هذا.المسار + اسم + "/"
        وإلا
            اطبع("❌ مجلد غير موجود: " + اسم)
        نهاية
    نهاية

    دالة اعرض()
        اطبع("📂 " + هذا.المسار)
        لكل عنصر في هذا.الحالي.قائمة()
            اطبع("  " + عنصر)
        نهاية
    نهاية
نهاية

متغير نم = جديد نظام_ملفات()
نم.أنشئ_مجلد("مستندات")
نم.أنشئ_مجلد("صور")
نم.أنشئ_ملف("ملاحظات.txt"، "مرحباً")
نم.اعرض()
نم.ادخل("مستندات")
نم.أنشئ_ملف("تقرير.txt"، "التقرير الشهري")
نم.اعرض()`,exercise:'أضف أمر "ارجع" للعودة للمجلد الأب وأمر "ابحث" للبحث العميق.',hint:"احفظ مكدس المجلدات السابقة للرجوع. للبحث استخدم العودية.",tips:["نظام الملفات = شجرة من مجلدات وملفات","التنقل يحتاج تتبع المسار الحالي","البحث العميق يستخدم العودية"],concepts:["نظام ملفات","شجرة","تنقل","بحث عميق"]},{id:"l4_59",title:"آلة حاسبة متقدمة",desc:"آلة حاسبة بسجل عمليات وذاكرة ودوال رياضية.",explanation:'الحاسبة المتقدمة أكثر من جمع وطرح: تسلسل العمليات يتيح كتابة ".set(10).add(5).multiply(2)" بوضوح. سجل العمليات يتيح التراجع (undo). الذاكرة تحفظ نتائج وسيطة. مشروع صغير يطبق كثير من الأنماط.',code:`# آلة حاسبة متقدمة
صنف حاسبة
    باني()
        هذا.نتيجة = 0
        هذا.تاريخ = []
        هذا.ذاكرة = 0
    نهاية

    دالة _سجّل(عملية، قيمة، نتيجة)
        هذا.تاريخ.أضف({
            "عملية": عملية،
            "قيمة": قيمة،
            "نتيجة": نتيجة
        })
    نهاية

    دالة عيّن(قيمة)
        هذا.نتيجة = قيمة
        هذا._سجّل("عيّن"، قيمة، قيمة)
        ارجع هذا
    نهاية

    دالة جمع(قيمة)
        هذا.نتيجة = هذا.نتيجة + قيمة
        هذا._سجّل("+"، قيمة، هذا.نتيجة)
        ارجع هذا
    نهاية

    دالة اطرح(قيمة)
        هذا.نتيجة = هذا.نتيجة - قيمة
        هذا._سجّل("-"، قيمة، هذا.نتيجة)
        ارجع هذا
    نهاية

    دالة اضرب(قيمة)
        هذا.نتيجة = هذا.نتيجة * قيمة
        هذا._سجّل("×"، قيمة، هذا.نتيجة)
        ارجع هذا
    نهاية

    دالة اقسم(قيمة)
        إذا (قيمة == 0) ارمي "لا يمكن القسمة على صفر" نهاية
        هذا.نتيجة = هذا.نتيجة / قيمة
        هذا._سجّل("÷"، قيمة، هذا.نتيجة)
        ارجع هذا
    نهاية

    دالة احفظ()
        هذا.ذاكرة = هذا.نتيجة
        ارجع هذا
    نهاية

    دالة استرجع()
        هذا.نتيجة = هذا.ذاكرة
        ارجع هذا
    نهاية

    دالة تراجع()
        إذا (هذا.تاريخ.طول() > 1)
            هذا.تاريخ = هذا.تاريخ.شريحة(0، هذا.تاريخ.طول() - 1)
            هذا.نتيجة = هذا.تاريخ[هذا.تاريخ.طول() - 1]["نتيجة"]
        نهاية
        ارجع هذا
    نهاية

    دالة القيمة()
        اطبع("= " + هذا.نتيجة)
        ارجع هذا.نتيجة
    نهاية
نهاية

متغير ح = جديد حاسبة()
ح.عيّن(100).جمع(50).اضرب(2).اطرح(30).القيمة()
# = 270
ح.تراجع().القيمة()
# = 300`,exercise:"أضف دوال رياضية: القوة (أُس)، الجذر التربيعي (تقريبي)، والنسبة المئوية.",hint:"الجذر التربيعي بطريقة نيوتن: كرر x = (x + n/x) / 2.",tips:["تسلسل الدوال يجعل الحسابات طبيعية","سجل العمليات يتيح التراجع","الذاكرة تحفظ نتائج وسيطة"],concepts:["حاسبة","تسلسل","تراجع","ذاكرة"]},{id:"l4_60",title:"تطبيق مهام (Todo)",desc:"تطبيق إدارة مهام كامل مع أولويات وتصفية وإحصائيات.",explanation:"تطبيق المهام المشروع الأول الكلاسيكي — بسيط لكن كامل. إضافة/إكمال/حذف = CRUD. الأولويات = ترتيب. الوسوم = تصنيف مرن. الإحصائيات = تجميع. إضافة المهام الفرعية تحوله لشجرة. مشروع تطبيقي حقيقي.",code:`# تطبيق إدارة المهام
صنف مدير_مهام
    باني()
        هذا.مهام = []
        هذا.رقم = 0
    نهاية

    دالة أضف(نص، أولوية)
        هذا.رقم = هذا.رقم + 1
        هذا.مهام.أضف({
            "رقم": هذا.رقم،
            "نص": نص،
            "أولوية": أولوية،
            "مكتمل": خطأ،
            "وسم": []
        })
        اطبع("✅ أُضيفت: #" + هذا.رقم + " " + نص)
    نهاية

    دالة أكمل(رقم)
        لكل م في هذا.مهام
            إذا (م["رقم"] == رقم)
                م["مكتمل"] = صحيح
                اطبع("✔️ اكتملت: " + م["نص"])
                ارجع صحيح
            نهاية
        نهاية
        ارجع خطأ
    نهاية

    دالة وسم(رقم، وسم)
        لكل م في هذا.مهام
            إذا (م["رقم"] == رقم)
                م["وسم"].أضف(وسم)
            نهاية
        نهاية
    نهاية

    دالة المعلقة()
        متغير نتيجة = []
        لكل م في هذا.مهام
            إذا (!م["مكتمل"])
                نتيجة.أضف(م)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية

    دالة حسب_أولوية(أولوية)
        متغير نتيجة = []
        لكل م في هذا.مهام
            إذا (م["أولوية"] == أولوية)
                نتيجة.أضف(م)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية

    دالة إحصائيات()
        متغير مكتمل = 0
        لكل م في هذا.مهام
            إذا (م["مكتمل"]) مكتمل = مكتمل + 1 نهاية
        نهاية
        اطبع("═══ إحصائيات ═══")
        اطبع("الكل: " + هذا.مهام.طول())
        اطبع("مكتمل: " + مكتمل)
        اطبع("معلّق: " + (هذا.مهام.طول() - مكتمل))
    نهاية

    دالة اعرض()
        لكل م في هذا.مهام
            متغير حالة = "⬜"
            إذا (م["مكتمل"]) حالة = "✅" نهاية
            متغير نجوم = ""
            لكل ت في [0..م["أولوية"] - 1]
                نجوم = نجوم + "⭐"
            نهاية
            اطبع(حالة + " #" + م["رقم"] + " " + م["نص"] + " " + نجوم)
        نهاية
    نهاية
نهاية

متغير مهامي = جديد مدير_مهام()
مهامي.أضف("تعلم هياكل البيانات"، 3)
مهامي.أضف("حل تمارين الخوارزميات"، 2)
مهامي.أضف("بناء مشروع"، 3)
مهامي.أضف("قراءة كتاب"، 1)
مهامي.أكمل(1)
مهامي.وسم(2، "برمجة")
مهامي.اعرض()
مهامي.إحصائيات()`,exercise:'أضف ميزة "المهام الفرعية" — كل مهمة يمكنها احتواء مهام داخلية.',hint:'أضف خاصية "فرعية" كمصفوفة لكل مهمة واعرضها بمسافة بادئة.',tips:["تطبيق المهام يجمع CRUD والبحث والعرض","الأولويات تساعد في ترتيب العمل","الوسوم تتيح تصنيفاً مرناً"],concepts:["إدارة مهام","CRUD","تصفية","ترتيب"]},{id:"l4_61",title:"نظام مكتبة",desc:"نظام إدارة مكتبة كتب مع الإعارة والبحث والتقارير.",explanation:"نظام المكتبة يجمع عدة أنماط: كيانات مترابطة (كتاب، عضو)، قواعد عمل (حد الإعارة)، حالات (متاح/معار). البحث في حقول متعددة (عنوان، مؤلف). مشروع يحاكي أنظمة الأعمال الحقيقية.",code:`# نظام إدارة مكتبة
صنف كتاب
    باني(عنوان، مؤلف، تصنيف)
        هذا.عنوان = عنوان
        هذا.مؤلف = مؤلف
        هذا.تصنيف = تصنيف
        هذا.متاح = صحيح
        هذا.مستعير = لاشيء
    نهاية
نهاية

صنف مكتبة
    باني(اسم)
        هذا.اسم = اسم
        هذا.كتب = []
        هذا.أعضاء = {}
    نهاية

    دالة أضف_كتاب(عنوان، مؤلف، تصنيف)
        هذا.كتب.أضف(جديد كتاب(عنوان، مؤلف، تصنيف))
    نهاية

    دالة سجّل_عضو(اسم)
        هذا.أعضاء[اسم] = {"كتب": []، "حد": 3}
    نهاية

    دالة أعر(عنوان، عضو)
        إذا (هذا.أعضاء[عضو] == لاشيء)
            اطبع("❌ عضو غير مسجل")
            ارجع خطأ
        نهاية
        إذا (هذا.أعضاء[عضو]["كتب"].طول() >= هذا.أعضاء[عضو]["حد"])
            اطبع("❌ تجاوز حد الإعارة")
            ارجع خطأ
        نهاية
        لكل كتاب في هذا.كتب
            إذا (كتاب.عنوان == عنوان و كتاب.متاح)
                كتاب.متاح = خطأ
                كتاب.مستعير = عضو
                هذا.أعضاء[عضو]["كتب"].أضف(عنوان)
                اطبع("📖 تم إعارة: " + عنوان + " لـ " + عضو)
                ارجع صحيح
            نهاية
        نهاية
        اطبع("❌ الكتاب غير متاح")
        ارجع خطأ
    نهاية

    دالة أرجع(عنوان، عضو)
        لكل كتاب في هذا.كتب
            إذا (كتاب.عنوان == عنوان و كتاب.مستعير == عضو)
                كتاب.متاح = صحيح
                كتاب.مستعير = لاشيء
                متغير جديدة = []
                لكل ك في هذا.أعضاء[عضو]["كتب"]
                    إذا (ك != عنوان) جديدة.أضف(ك) نهاية
                نهاية
                هذا.أعضاء[عضو]["كتب"] = جديدة
                اطبع("📗 تم الإرجاع: " + عنوان)
                ارجع صحيح
            نهاية
        نهاية
        ارجع خطأ
    نهاية

    دالة ابحث(كلمة)
        متغير نتائج = []
        لكل كتاب في هذا.كتب
            إذا (كتاب.عنوان.يحتوي(كلمة) أو كتاب.مؤلف.يحتوي(كلمة))
                نتائج.أضف(كتاب)
            نهاية
        نهاية
        ارجع نتائج
    نهاية
نهاية

متغير مكتبتي = جديد مكتبة("المكتبة المركزية")
مكتبتي.أضف_كتاب("البرمجة بلغة ص"، "أحمد"، "تقنية")
مكتبتي.أضف_كتاب("الخوارزميات"، "محمد"، "تقنية")
مكتبتي.سجّل_عضو("سارة")
مكتبتي.أعر("البرمجة بلغة ص"، "سارة")
مكتبتي.أعر("الخوارزميات"، "سارة")`,exercise:"أضف نظام غرامات للتأخر في الإرجاع ونظام حجز للكتب غير المتاحة.",hint:"أضف تاريخ إعارة وقارنه بتاريخ الإرجاع لحساب الغرامة.",tips:["نظام المكتبة يجمع عدة أنماط تصميم","التحقق من الحدود يمنع أخطاء المنطق","البحث في عدة حقول يعطي نتائج أشمل"],concepts:["نظام مكتبة","إعارة","بحث","قيود"]},{id:"l4_62",title:"محرك لعبة بسيط",desc:"بناء محرك لعبة نصية مع كيانات وحالة وتحديث.",explanation:"محرك اللعبة حلقة أساسية: مدخلات → تحديث → رسم → تكرار. الكيانات مستقلة (لاعب، عدو، كنز) والعالم ينسق بينها. كشف التصادم أساسي. حتى اللعبة النصية تعلمك مبادئ الألعاب الرسومية.",code:`# محرك لعبة نصية بسيط
صنف كيان
    باني(اسم، رمز، س، ص)
        هذا.اسم = اسم
        هذا.رمز = رمز
        هذا.س = س
        هذا.ص = ص
        هذا.صحة = 100
        هذا.نشط = صحيح
    نهاية
نهاية

صنف عالم_لعبة
    باني(عرض، ارتفاع)
        هذا.عرض = عرض
        هذا.ارتفاع = ارتفاع
        هذا.كيانات = []
        هذا.دورة = 0
    نهاية

    دالة أضف(كيان)
        هذا.كيانات.أضف(كيان)
    نهاية

    دالة حرّك(كيان، دس، دص)
        متغير س_جديد = كيان.س + دس
        متغير ص_جديد = كيان.ص + دص
        إذا (س_جديد >= 0 و س_جديد < هذا.عرض و ص_جديد >= 0 و ص_جديد < هذا.ارتفاع)
            كيان.س = س_جديد
            كيان.ص = ص_جديد
        نهاية
    نهاية

    دالة تصادم(ك1، ك2)
        ارجع ك1.س == ك2.س و ك1.ص == ك2.ص و ك1.نشط و ك2.نشط
    نهاية

    دالة حدّث()
        هذا.دورة = هذا.دورة + 1
        # فحص التصادمات
        لكل ت في [0..هذا.كيانات.طول() - 1]
            لكل ج في [ت + 1..هذا.كيانات.طول() - 1]
                إذا (هذا.تصادم(هذا.كيانات[ت]، هذا.كيانات[ج]))
                    اطبع("💥 تصادم: " + هذا.كيانات[ت].اسم + " و " + هذا.كيانات[ج].اسم)
                نهاية
            نهاية
        نهاية
    نهاية

    دالة ارسم()
        اطبع("--- دورة " + هذا.دورة + " ---")
        لكل ص في [0..هذا.ارتفاع - 1]
            متغير سطر = ""
            لكل س في [0..هذا.عرض - 1]
                متغير رمز = "·"
                لكل ك في هذا.كيانات
                    إذا (ك.س == س و ك.ص == ص و ك.نشط)
                        رمز = ك.رمز
                    نهاية
                نهاية
                سطر = سطر + رمز
            نهاية
            اطبع(سطر)
        نهاية
    نهاية
نهاية

متغير لعبة = جديد عالم_لعبة(10، 5)
متغير لاعب = جديد كيان("البطل"، "🧑"، 0، 2)
متغير عدو = جديد كيان("وحش"، "👾"، 8، 2)
متغير كنز = جديد كيان("كنز"، "💎"، 5، 2)
لعبة.أضف(لاعب)
لعبة.أضف(عدو)
لعبة.أضف(كنز)
لعبة.ارسم()
لعبة.حرّك(لاعب، 2، 0)
لعبة.حدّث()
لعبة.ارسم()`,exercise:"أضف نظام نقاط ومستويات — عند جمع الكنز زد النقاط وأنشئ كنزاً جديداً.",hint:"تحقق من تصادم اللاعب والكنز واستخدم عدّاداً للنقاط.",tips:["محرك اللعبة = حلقة: مدخلات → تحديث → رسم","كشف التصادمات أساسي في كل لعبة","الكيانات مستقلة والعالم ينسق بينها"],concepts:["محرك لعبة","كيانات","تصادم","حلقة لعبة"]}]},Cc={id:"l4_advanced_challenges",title:"تحديات متقدمة",icon:"🏆",lessons:[{id:"l4_63",title:"تحدي: مفسر عبارات منطقية",desc:"بناء مفسر يقيّم عبارات منطقية مركبة مع المتغيرات.",explanation:'المنطق أساس البرمجة: "أ و (ب أو ليس ج)" كيف تُقيّم؟ بناء مفسر منطقي يعلمك التحليل بالنزول التكراري — الأسلوب المستخدم في مترجمات اللغات الحقيقية. كل مستوى أولوية (ليس > و > أو) له دالته.',code:`# تحدي: مفسر عبارات منطقية
# يدعم: صحيح، خطأ، و، أو، ليس، أقواس

صنف مفسر_منطق
    باني()
        هذا.متغيرات = {}
    نهاية

    دالة عيّن(اسم، قيمة)
        هذا.متغيرات[اسم] = قيمة
    نهاية

    دالة قيّم(تعبير)
        متغير رموز = تعبير.قسم(" ")
        متغير موقع = {"ت": 0}
        ارجع هذا._أو(رموز، موقع)
    نهاية

    دالة _أو(رموز، موقع)
        متغير يسار = هذا._و(رموز، موقع)
        بينما (موقع["ت"] < رموز.طول() و رموز[موقع["ت"]] == "أو")
            موقع["ت"] = موقع["ت"] + 1
            متغير يمين = هذا._و(رموز، موقع)
            يسار = يسار أو يمين
        نهاية
        ارجع يسار
    نهاية

    دالة _و(رموز، موقع)
        متغير يسار = هذا._لا(رموز، موقع)
        بينما (موقع["ت"] < رموز.طول() و رموز[موقع["ت"]] == "و")
            موقع["ت"] = موقع["ت"] + 1
            متغير يمين = هذا._لا(رموز، موقع)
            يسار = يسار و يمين
        نهاية
        ارجع يسار
    نهاية

    دالة _لا(رموز، موقع)
        إذا (موقع["ت"] < رموز.طول() و رموز[موقع["ت"]] == "ليس")
            موقع["ت"] = موقع["ت"] + 1
            ارجع !هذا._أساسي(رموز، موقع)
        نهاية
        ارجع هذا._أساسي(رموز، موقع)
    نهاية

    دالة _أساسي(رموز، موقع)
        متغير رمز = رموز[موقع["ت"]]
        موقع["ت"] = موقع["ت"] + 1
        إذا (رمز == "صحيح") ارجع صحيح نهاية
        إذا (رمز == "خطأ") ارجع خطأ نهاية
        إذا (هذا.متغيرات[رمز] != لاشيء)
            ارجع هذا.متغيرات[رمز]
        نهاية
        ارجع خطأ
    نهاية
نهاية

متغير م = جديد مفسر_منطق()
م.عيّن("أ"، صحيح)
م.عيّن("ب"، خطأ)
اطبع("أ و ب = " + م.قيّم("أ و ب"))             # خطأ
اطبع("أ أو ب = " + م.قيّم("أ أو ب"))           # صحيح
اطبع("ليس ب = " + م.قيّم("ليس ب"))             # صحيح
اطبع("أ و ليس ب = " + م.قيّم("أ و ليس ب"))     # صحيح`,exercise:"أضف دعم الأقواس () لتجميع العبارات ودعم عامل التضمين (==>).",hint:'A ==> B يعادل (ليس A) أو B. للأقواس: عند "(" استدع _أو وعند ")" ارجع.',tips:["بناء محلل بالنزول التكراري (Recursive Descent)","الأولويات: ليس > و > أو","المفسر يحلل ويقيّم في نفس الوقت"],concepts:["مفسر","منطق","تحليل تكراري","أولويات"]},{id:"l4_64",title:"تحدي: نظام أحداث متقدم",desc:"بناء نظام أحداث مع فقاعات وإلغاء واشتراكات مشروطة.",explanation:"نظام الأحداث المتقدم يتجاوز الاشتراك البسيط: الاشتراك لمرة واحدة (عرض ترحيبي)، إلغاء الحدث (إيقاف المعالجين التاليين)، wildcards (استقبل كل الأحداث). أساس React/Vue ومعالجة DOM وأنظمة المراسلة.",code:`# نظام أحداث متقدم
صنف مدير_أحداث
    باني()
        هذا.مستمعون = {}
        هذا.مستمعون_مرة = {}
    نهاية

    دالة عند(حدث، دالة_استجابة)
        إذا (هذا.مستمعون[حدث] == لاشيء)
            هذا.مستمعون[حدث] = []
        نهاية
        هذا.مستمعون[حدث].أضف(دالة_استجابة)
    نهاية

    دالة مرة(حدث، دالة_استجابة)
        إذا (هذا.مستمعون_مرة[حدث] == لاشيء)
            هذا.مستمعون_مرة[حدث] = []
        نهاية
        هذا.مستمعون_مرة[حدث].أضف(دالة_استجابة)
    نهاية

    دالة أطلق(حدث، بيانات)
        متغير تم_إلغاء = خطأ
        متغير سياق = {
            "حدث": حدث،
            "بيانات": بيانات،
            "ألغِ": لامدا () تم_إلغاء = صحيح نهاية
        }

        # مستمعون دائمون
        إذا (هذا.مستمعون[حدث] != لاشيء)
            لكل مستمع في هذا.مستمعون[حدث]
                إذا (تم_إلغاء) توقف نهاية
                مستمع(سياق)
            نهاية
        نهاية

        # مستمعون لمرة واحدة
        إذا (هذا.مستمعون_مرة[حدث] != لاشيء)
            لكل مستمع في هذا.مستمعون_مرة[حدث]
                إذا (تم_إلغاء) توقف نهاية
                مستمع(سياق)
            نهاية
            هذا.مستمعون_مرة[حدث] = []
        نهاية
    نهاية

    دالة أحداث_مسجلة()
        متغير أحداث = []
        لكل حدث في هذا.مستمعون
            أحداث.أضف(حدث)
        نهاية
        ارجع أحداث
    نهاية
نهاية

# تطبيق: نظام تسوق
متغير أحداث = جديد مدير_أحداث()
أحداث.عند("شراء"، لامدا (س)
    اطبع("📧 إرسال إيصال: " + س["بيانات"]["منتج"])
نهاية)
أحداث.عند("شراء"، لامدا (س)
    اطبع("📊 تحديث المخزون")
نهاية)
أحداث.مرة("شراء"، لامدا (س)
    اطبع("🎉 خصم 10% على الشراء التالي (مرة واحدة)")
نهاية)

أحداث.أطلق("شراء"، {"منتج": "هاتف"، "سعر": 3000})
اطبع("--- الشراء الثاني ---")
أحداث.أطلق("شراء"، {"منتج": "سماعة"، "سعر": 200})`,exercise:'أضف ميزة wildcards — الاشتراك بـ "*" يستقبل جميع الأحداث.',hint:'في دالة أطلق()، نفّذ مستمعي "*" إضافة لمستمعي الحدث المحدد.',tips:["نظام الأحداث يفصل الناشر عن المستقبل","الاشتراك لمرة واحدة مناسب للعروض","إلغاء الحدث يوقف تنفيذ المستمعين التاليين"],concepts:["أحداث","Event System","اشتراك","إلغاء"]},{id:"l4_65",title:"تحدي: نظام كاش (Cache)",desc:"بناء نظام تخزين مؤقت بسياسة LRU.",explanation:"الكاش يسرع كل شيء: بدلاً من حساب نتيجة كل مرة، احفظها وأعدها. LRU (الأقل استخداماً) يزيل القديم عند الامتلاء. نسبة الإصابة تقيس فعالية الكاش. مستخدم في كل مكان: المتصفح، قواعد البيانات، المعالج.",code:`# نظام كاش LRU — الأقل استخداماً يُزال أولاً
صنف كاش_LRU
    باني(سعة)
        هذا.سعة = سعة
        هذا.بيانات = {}
        هذا.ترتيب = []  # الأحدث في النهاية
        هذا.إصابات = 0
        هذا.أخطاء = 0
    نهاية

    دالة _حدّث_ترتيب(مفتاح)
        # أزل المفتاح من موقعه الحالي
        متغير جديد = []
        لكل م في هذا.ترتيب
            إذا (م != مفتاح)
                جديد.أضف(م)
            نهاية
        نهاية
        جديد.أضف(مفتاح)
        هذا.ترتيب = جديد
    نهاية

    دالة اقرأ(مفتاح)
        إذا (هذا.بيانات[مفتاح] != لاشيء)
            هذا.إصابات = هذا.إصابات + 1
            هذا._حدّث_ترتيب(مفتاح)
            ارجع هذا.بيانات[مفتاح]
        نهاية
        هذا.أخطاء = هذا.أخطاء + 1
        ارجع لاشيء
    نهاية

    دالة اكتب(مفتاح، قيمة)
        إذا (هذا.بيانات[مفتاح] != لاشيء)
            هذا.بيانات[مفتاح] = قيمة
            هذا._حدّث_ترتيب(مفتاح)
            ارجع "تحديث"
        نهاية
        # إزالة الأقدم إذا امتلأ
        إذا (هذا.ترتيب.طول() >= هذا.سعة)
            متغير أقدم = هذا.ترتيب[0]
            هذا.ترتيب = هذا.ترتيب.شريحة(1)
            هذا.بيانات[أقدم] = لاشيء
        نهاية
        هذا.بيانات[مفتاح] = قيمة
        هذا.ترتيب.أضف(مفتاح)
        ارجع "إضافة"
    نهاية

    دالة إحصائيات()
        متغير مجموع = هذا.إصابات + هذا.أخطاء
        متغير نسبة = 0
        إذا (مجموع > 0) نسبة = (هذا.إصابات * 100) / مجموع نهاية
        اطبع("═══ كاش ═══")
        اطبع("سعة: " + هذا.سعة + " | مستخدم: " + هذا.ترتيب.طول())
        اطبع("إصابات: " + هذا.إصابات + " | أخطاء: " + هذا.أخطاء)
        اطبع("نسبة الإصابة: " + نسبة + "%")
    نهاية
نهاية

متغير كاش = جديد كاش_LRU(3)
كاش.اكتب("أ"، "بيانات أ")
كاش.اكتب("ب"، "بيانات ب")
كاش.اكتب("ج"، "بيانات ج")
كاش.اقرأ("أ")              # إصابة
كاش.اكتب("د"، "بيانات د")  # يزيل "ب" (الأقل استخداماً)
اطبع(كاش.اقرأ("ب"))        # لاشيء — أُزيل
اطبع(كاش.اقرأ("أ"))        # بيانات أ — لا يزال موجوداً
كاش.إحصائيات()`,exercise:"أضف سياسة TTL — كل عنصر له وقت انتهاء صلاحية.",hint:"أضف طابعاً زمنياً لكل عنصر وتحقق منه عند القراءة.",tips:["LRU يزيل الأقل استخداماً أولاً","الكاش يسرع الوصول للبيانات المتكررة","نسبة الإصابة تقيس فعالية الكاش"],concepts:["كاش","LRU","Cache","أداء"]},{id:"l4_66",title:"تحدي: جدولة المهام",desc:"بناء نظام جدولة مهام بتبعيات — تنفيذ بالترتيب الصحيح.",explanation:"بعض المهام تعتمد على أخرى: البناء يحتاج التحميل أولاً. الترتيب الطوبولوجي يحترم التبعيات ويكشف الدورات (أ يحتاج ب يحتاج أ = مستحيل). أساس أدوات مثل Make, npm scripts, CI/CD pipelines.",code:`# جدولة مهام بتبعيات — Topological Sort
صنف مجدول_مهام
    باني()
        هذا.مهام = {}
        هذا.تبعيات = {}
    نهاية

    دالة أضف_مهمة(اسم، دالة_تنفيذ)
        هذا.مهام[اسم] = دالة_تنفيذ
        إذا (هذا.تبعيات[اسم] == لاشيء)
            هذا.تبعيات[اسم] = []
        نهاية
    نهاية

    دالة تعتمد_على(مهمة، تعتمد)
        إذا (هذا.تبعيات[مهمة] == لاشيء)
            هذا.تبعيات[مهمة] = []
        نهاية
        هذا.تبعيات[مهمة].أضف(تعتمد)
    نهاية

    دالة _ترتيب_طوبولوجي()
        متغير ترتيب = []
        متغير زيارة = {}
        متغير قيد_الزيارة = {}

        لكل مهمة في هذا.مهام
            إذا (زيارة[مهمة] != صحيح)
                هذا._زر(مهمة، زيارة، قيد_الزيارة، ترتيب)
            نهاية
        نهاية
        ارجع ترتيب
    نهاية

    دالة _زر(مهمة، زيارة، قيد_الزيارة، ترتيب)
        إذا (قيد_الزيارة[مهمة] == صحيح)
            ارمي "تبعية دائرية عند: " + مهمة
        نهاية
        إذا (زيارة[مهمة] == صحيح)
            ارجع لاشيء
        نهاية
        قيد_الزيارة[مهمة] = صحيح
        إذا (هذا.تبعيات[مهمة] != لاشيء)
            لكل تبعية في هذا.تبعيات[مهمة]
                هذا._زر(تبعية، زيارة، قيد_الزيارة، ترتيب)
            نهاية
        نهاية
        قيد_الزيارة[مهمة] = خطأ
        زيارة[مهمة] = صحيح
        ترتيب.أضف(مهمة)
    نهاية

    دالة نفّذ_الكل()
        متغير ترتيب = هذا._ترتيب_طوبولوجي()
        اطبع("ترتيب التنفيذ: " + ترتيب)
        لكل مهمة في ترتيب
            اطبع("⚙️ تنفيذ: " + مهمة)
            هذا.مهام[مهمة]()
        نهاية
    نهاية
نهاية

# مثال: بناء مشروع
متغير مجدول = جديد مجدول_مهام()
مجدول.أضف_مهمة("تنظيف"، لامدا () اطبع("  🧹 تنظيف الملفات القديمة") نهاية)
مجدول.أضف_مهمة("تحميل"، لامدا () اطبع("  📥 تحميل المكتبات") نهاية)
مجدول.أضف_مهمة("بناء"، لامدا () اطبع("  🔨 بناء المشروع") نهاية)
مجدول.أضف_مهمة("اختبار"، لامدا () اطبع("  🧪 تشغيل الاختبارات") نهاية)
مجدول.أضف_مهمة("نشر"، لامدا () اطبع("  🚀 نشر المشروع") نهاية)

مجدول.تعتمد_على("بناء"، "تنظيف")
مجدول.تعتمد_على("بناء"، "تحميل")
مجدول.تعتمد_على("اختبار"، "بناء")
مجدول.تعتمد_على("نشر"، "اختبار")

مجدول.نفّذ_الكل()`,exercise:"أضف تنفيذ متوازي — المهام المستقلة تُنفذ معاً.",hint:"حلل التبعيات وحدد المهام التي ليس لها تبعيات غير منفذة.",tips:["الترتيب الطوبولوجي يحترم التبعيات","كشف الدورات يمنع التعليق اللانهائي","مستخدم في أنظمة البناء وCI/CD"],concepts:["جدولة","تبعيات","ترتيب طوبولوجي","DAG"]},{id:"l4_67",title:"تحدي: آلة حالة محدودة (FSM)",desc:"بناء آلة حالة عامة قابلة للتكوين مع إجراءات انتقال.",explanation:'آلة الحالة تنظم السلوك المعقد: الباب "مغلق" → "يفتح" → "مفتوح" → "يغلق". كل حالة + حدث = انتقال محدد. الإجراءات تُنفذ عند الانتقال ("عائق!" عند التوقف). مستخدم في الألعاب، واجهات المستخدم، وبروتوكولات الشبكات.',code:`# آلة حالة محدودة — Finite State Machine
صنف آلة_حالة
    باني(اسم)
        هذا.اسم = اسم
        هذا.حالة = لاشيء
        هذا.حالات = {}
        هذا.انتقالات = {}
        هذا.سجل = []
    نهاية

    دالة أضف_حالة(اسم، عند_الدخول)
        هذا.حالات[اسم] = {"اسم": اسم، "دخول": عند_الدخول}
        إذا (هذا.حالة == لاشيء)
            هذا.حالة = اسم
            إذا (عند_الدخول != لاشيء) عند_الدخول() نهاية
        نهاية
    نهاية

    دالة أضف_انتقال(من، حدث، إلى، إجراء)
        متغير مفتاح = من + ":" + حدث
        هذا.انتقالات[مفتاح] = {"إلى": إلى، "إجراء": إجراء}
    نهاية

    دالة أرسل(حدث)
        متغير مفتاح = هذا.حالة + ":" + حدث
        متغير انتقال = هذا.انتقالات[مفتاح]
        إذا (انتقال == لاشيء)
            اطبع("⚠️ لا انتقال من " + هذا.حالة + " بحدث " + حدث)
            ارجع خطأ
        نهاية
        هذا.سجل.أضف(هذا.حالة + " → " + انتقال["إلى"])
        هذا.حالة = انتقال["إلى"]
        إذا (انتقال["إجراء"] != لاشيء)
            انتقال["إجراء"]()
        نهاية
        متغير حالة_جديدة = هذا.حالات[هذا.حالة]
        إذا (حالة_جديدة != لاشيء و حالة_جديدة["دخول"] != لاشيء)
            حالة_جديدة["دخول"]()
        نهاية
        ارجع صحيح
    نهاية

    دالة الحالة_الحالية()
        ارجع هذا.حالة
    نهاية
نهاية

# باب آلي
متغير باب = جديد آلة_حالة("باب آلي")
باب.أضف_حالة("مغلق"، لامدا () اطبع("🚪 الباب مغلق") نهاية)
باب.أضف_حالة("يفتح"، لامدا () اطبع("🔄 الباب يفتح...") نهاية)
باب.أضف_حالة("مفتوح"، لامدا () اطبع("🚪 الباب مفتوح") نهاية)
باب.أضف_حالة("يغلق"، لامدا () اطبع("🔄 الباب يغلق...") نهاية)

باب.أضف_انتقال("مغلق"، "فتح"، "يفتح"، لاشيء)
باب.أضف_انتقال("يفتح"، "اكتمل"، "مفتوح"، لاشيء)
باب.أضف_انتقال("مفتوح"، "إغلاق"، "يغلق"، لاشيء)
باب.أضف_انتقال("يغلق"، "اكتمل"، "مغلق"، لاشيء)
باب.أضف_انتقال("يغلق"، "عائق"، "يفتح"، لامدا () اطبع("⚠️ عائق!") نهاية)

باب.أرسل("فتح")
باب.أرسل("اكتمل")
باب.أرسل("إغلاق")
باب.أرسل("عائق")
باب.أرسل("اكتمل")`,exercise:"أنشئ آلة حالة لنظام مصعد بعدة طوابق.",hint:"الحالات: متوقف، صاعد، نازل. الأحداث: طلب_طابق مع رقم.",tips:["آلة الحالة تنظم السلوك المعقد","كل حالة + حدث = انتقال محدد","الإجراءات تُنفذ عند الانتقال"],concepts:["آلة حالة","FSM","انتقالات","أحداث"]},{id:"l4_68",title:"تحدي: محرك قواعد عمل",desc:"بناء محرك قواعد يطبق سياسات العمل على بيانات.",explanation:'قواعد العمل تتغير باستمرار: "خصم VIP 20%" قد يصبح 25% غداً. محرك القواعد يفصل المنطق عن الكود — القواعد بيانات قابلة للتعديل دون إعادة برمجة. الأولويات تحدد ترتيب التطبيق. مستخدم في Drools وأنظمة الخصومات.',code:`# محرك قواعد عمل — Business Rules Engine
صنف قاعدة
    باني(اسم، شرط، إجراء، أولوية)
        هذا.اسم = اسم
        هذا.شرط = شرط
        هذا.إجراء = إجراء
        هذا.أولوية = أولوية
    نهاية
نهاية

صنف محرك_قواعد
    باني()
        هذا.قواعد = []
    نهاية

    دالة أضف(اسم، شرط، إجراء، أولوية)
        هذا.قواعد.أضف(جديد قاعدة(اسم، شرط، إجراء، أولوية))
        # ترتيب حسب الأولوية
        متغير ن = هذا.قواعد.طول()
        لكل ت في [0..ن - 1]
            لكل ج في [0..ن - ت - 2]
                إذا (هذا.قواعد[ج].أولوية < هذا.قواعد[ج + 1].أولوية)
                    متغير مؤقت = هذا.قواعد[ج]
                    هذا.قواعد[ج] = هذا.قواعد[ج + 1]
                    هذا.قواعد[ج + 1] = مؤقت
                نهاية
            نهاية
        نهاية
    نهاية

    دالة طبّق(بيانات)
        متغير نتائج = []
        لكل قاعدة في هذا.قواعد
            إذا (قاعدة.شرط(بيانات))
                متغير نتيجة = قاعدة.إجراء(بيانات)
                نتائج.أضف({"قاعدة": قاعدة.اسم، "نتيجة": نتيجة})
                اطبع("✅ " + قاعدة.اسم)
            نهاية
        نهاية
        ارجع نتائج
    نهاية
نهاية

# نظام خصومات متجر
متغير محرك = جديد محرك_قواعد()
محرك.أضف("خصم VIP"،
    لامدا (ط) ارجع ط["عضوية"] == "VIP" نهاية،
    لامدا (ط) ط["خصم"] = 20 ارجع "خصم 20% لعضو VIP" نهاية، 10)
محرك.أضف("خصم كمية"،
    لامدا (ط) ارجع ط["كمية"] > 5 نهاية،
    لامدا (ط) ط["خصم_إضافي"] = 5 ارجع "خصم 5% إضافي للكمية" نهاية، 5)
محرك.أضف("شحن مجاني"،
    لامدا (ط) ارجع ط["المجموع"] > 500 نهاية،
    لامدا (ط) ط["شحن"] = 0 ارجع "شحن مجاني!" نهاية، 3)

متغير طلب = {"عضوية": "VIP"، "كمية": 8، "المجموع": 800}
متغير نتائج = محرك.طبّق(طلب)
اطبع("القواعد المُطبقة: " + نتائج.طول())`,exercise:'أضف دعم "القاعدة الأولى فقط" — التوقف بعد أول قاعدة مطابقة.',hint:'أضف معامل "أول_فقط" لدالة طبّق وتوقف بعد أول تطابق.',tips:["محرك القواعد يفصل منطق العمل عن الكود","الأولويات تحدد ترتيب تطبيق القواعد","القواعد قابلة للتعديل دون تغيير الكود"],concepts:["محرك قواعد","Business Rules","سياسات"]},{id:"l4_69",title:"تحدي: بناء نظام إضافات (Plugins)",desc:"بناء نظام قابل للتوسع عبر إضافات مستقلة.",explanation:"الإضافات تجعل البرنامج قابلاً للتوسع دون تعديل النواة: مثل إضافات VS Code أو ووردبريس. الخطافات (hooks) تربط الإضافة بنقاط المعالجة. كل إضافة مستقلة ولا تعرف عن الأخرى. نمط شائع في التطبيقات الاحترافية.",code:`# نظام إضافات — Plugin System
صنف نظام_إضافات
    باني()
        هذا.إضافات = []
        هذا.خطافات = {}
    نهاية

    دالة سجّل(اسم، إعداد، تنظيف)
        هذا.إضافات.أضف({
            "اسم": اسم،
            "إعداد": إعداد،
            "تنظيف": تنظيف،
            "نشط": خطأ
        })
    نهاية

    دالة سجّل_خطاف(اسم_خطاف، دالة_معالجة)
        إذا (هذا.خطافات[اسم_خطاف] == لاشيء)
            هذا.خطافات[اسم_خطاف] = []
        نهاية
        هذا.خطافات[اسم_خطاف].أضف(دالة_معالجة)
    نهاية

    دالة نفّذ_خطاف(اسم، بيانات)
        إذا (هذا.خطافات[اسم] != لاشيء)
            لكل معالج في هذا.خطافات[اسم]
                بيانات = معالج(بيانات)
            نهاية
        نهاية
        ارجع بيانات
    نهاية

    دالة فعّل_الكل()
        لكل إضافة في هذا.إضافات
            اطبع("🔌 تفعيل: " + إضافة["اسم"])
            إضافة["إعداد"](هذا)
            إضافة["نشط"] = صحيح
        نهاية
    نهاية

    دالة قائمة()
        لكل إ في هذا.إضافات
            متغير حالة = "❌"
            إذا (إ["نشط"]) حالة = "✅" نهاية
            اطبع(حالة + " " + إ["اسم"])
        نهاية
    نهاية
نهاية

# تطبيق: معالج نصوص بإضافات
متغير نظام = جديد نظام_إضافات()

# إضافة الحروف الكبيرة
نظام.سجّل("تكبير"،
    لامدا (ن) ن.سجّل_خطاف("معالجة"، لامدا (نص)
        اطبع("  📝 تحويل: " + نص)
        ارجع نص
    نهاية) نهاية،
    لامدا () اطبع("تنظيف تكبير") نهاية)

# إضافة إحصائيات
نظام.سجّل("إحصائيات"،
    لامدا (ن) ن.سجّل_خطاف("معالجة"، لامدا (نص)
        اطبع("  📊 كلمات: " + نص.قسم(" ").طول())
        ارجع نص
    نهاية) نهاية،
    لامدا () اطبع("تنظيف إحصائيات") نهاية)

نظام.فعّل_الكل()
نظام.قائمة()
متغير نتيجة = نظام.نفّذ_خطاف("معالجة"، "مرحباً بالعالم العربي")
اطبع("النتيجة: " + نتيجة)`,exercise:"أضف ميزة تعطيل/تفعيل إضافات فردية وترتيب أولوية الإضافات.",hint:'أضف خاصية "أولوية" ورتّب الإضافات بها عند التفعيل.',tips:["نظام الإضافات يجعل البرنامج قابلاً للتوسع","الخطافات تربط الإضافات بنقاط المعالجة","كل إضافة مستقلة ولا تعرف عن الأخرى"],concepts:["إضافات","Plugins","خطافات","Hooks"]},{id:"l4_70",title:"المشروع الختامي: إطار عمل مصغر",desc:"بناء إطار عمل مصغر يجمع كل مفاهيم المستوى المتقدم.",explanation:"هذا المشروع يجمع كل ما تعلمته: أنماط التصميم (مفرد، مراقب، مصنع)، نظام وحدات، نظام أحداث، إعدادات. الوحدات المستقلة تتيح التطوير التدريجي. الأحداث تربط الأجزاء بمرونة. انتهيت الآن من بناء إطار عمل حقيقي!",code:`# المشروع الختامي: إطار عمل مصغر
# يجمع: أنماط تصميم + هياكل بيانات + معالجة أخطاء

صنف إطار_عمل
    باني(اسم)
        هذا.اسم = اسم
        هذا.وحدات = {}
        هذا.أحداث = {}
        هذا.سجل_أحداث = []
        هذا.إعدادات = {}
    نهاية

    # نظام الوحدات
    دالة سجّل_وحدة(اسم، وحدة)
        هذا.وحدات[اسم] = وحدة
        هذا._أطلق("وحدة_مسجلة"، اسم)
    نهاية

    دالة وحدة(اسم)
        إذا (هذا.وحدات[اسم] == لاشيء)
            ارمي "وحدة غير موجودة: " + اسم
        نهاية
        ارجع هذا.وحدات[اسم]
    نهاية

    # نظام الأحداث
    دالة عند(حدث، معالج)
        إذا (هذا.أحداث[حدث] == لاشيء)
            هذا.أحداث[حدث] = []
        نهاية
        هذا.أحداث[حدث].أضف(معالج)
    نهاية

    دالة _أطلق(حدث، بيانات)
        هذا.سجل_أحداث.أضف(حدث + ": " + بيانات)
        إذا (هذا.أحداث[حدث] != لاشيء)
            لكل م في هذا.أحداث[حدث]
                م(بيانات)
            نهاية
        نهاية
    نهاية

    # نظام الإعدادات
    دالة عيّن(مفتاح، قيمة)
        هذا.إعدادات[مفتاح] = قيمة
    نهاية

    دالة اقرأ(مفتاح، افتراضي)
        إذا (هذا.إعدادات[مفتاح] != لاشيء)
            ارجع هذا.إعدادات[مفتاح]
        نهاية
        ارجع افتراضي
    نهاية

    # بدء وإيقاف
    دالة ابدأ()
        اطبع("🚀 بدء " + هذا.اسم)
        هذا._أطلق("بدء"، هذا.اسم)
        لكل اسم في هذا.وحدات
            اطبع("  📦 تحميل وحدة: " + اسم)
        نهاية
        هذا._أطلق("جاهز"، هذا.اسم)
    نهاية

    دالة حالة()
        اطبع("═══ " + هذا.اسم + " ═══")
        اطبع("الوحدات: ")
        لكل اسم في هذا.وحدات
            اطبع("  📦 " + اسم)
        نهاية
        اطبع("الأحداث المسجلة: " + هذا.سجل_أحداث.طول())
    نهاية
نهاية

# بناء تطبيق بالإطار
متغير تطبيق = جديد إطار_عمل("تطبيقي")

# تسجيل وحدات
تطبيق.سجّل_وحدة("سجل"، {
    "أضف": لامدا (رسالة) اطبع("📝 " + رسالة) نهاية
})
تطبيق.سجّل_وحدة("بيانات"، {
    "اقرأ": لامدا () ارجع [1، 2، 3] نهاية
})

# ربط أحداث
تطبيق.عند("بدء"، لامدا (اسم) اطبع("🎉 " + اسم + " يعمل!") نهاية)
تطبيق.عند("جاهز"، لامدا (اسم) اطبع("✅ " + اسم + " جاهز للاستخدام") نهاية)

# إعدادات
تطبيق.عيّن("وضع"، "تطوير")
تطبيق.عيّن("منفذ"، 3000)

# بدء
تطبيق.ابدأ()
تطبيق.حالة()
اطبع("الوضع: " + تطبيق.اقرأ("وضع"، "إنتاج"))`,exercise:"وسّع الإطار بـ middleware، كاش، ونظام إضافات من الدروس السابقة.",hint:"أنشئ وحدات مستقلة لكل ميزة وسجّلها في الإطار.",tips:["الإطار يجمع أنماط: مفرد، مراقب، مصنع","الوحدات المستقلة تتيح تطويراً تدريجياً","نظام الأحداث يربط الأجزاء بمرونة"],concepts:["إطار عمل","Framework","وحدات","أحداث","إعدادات"]}]},Lc={id:4,title:"المتقدم",titleEn:"Advanced",description:"أنماط التصميم، هياكل البيانات، الخوارزميات، ومشاريع تطبيقية.",icon:"🔥",color:"#f59e0b",sections:[gc,xc,kc,bc,yc,$c,Sc,Cc]},Pc={id:"l5_functional_advanced",title:"البرمجة الوظيفية المتقدمة",icon:"🧮",lessons:[{id:"l5_01",title:"الدوال من الرتبة العليا",desc:"دوال تأخذ دوالاً أخرى كمعاملات أو تُرجعها كنتائج لبناء كود مرن.",explanation:'الدوال من الرتبة العليا هي حجر الأساس في البرمجة الوظيفية الحديثة، وهي تعامل الدوال كمواطنين من الدرجة الأولى. تخيل أنك مدير مصنع: بدلاً من تكرار نفس العملية بنفسك، تعطي التعليمات (الدالة) للعمال ليطبقوها على كل قطعة. هذا النمط يفصل "ماذا نفعل" عن "كيف نكرر"، مما يجعل الكود أكثر قابلية لإعادة الاستخدام والاختبار. تجدها في كل مكتبات JavaScript الحديثة مثل map وfilter وreduce.',code:`# دوال من الرتبة العليا
دالة طبّق(قائمة، عملية)
    متغير نتائج = []
    لكل عنصر في قائمة
        نتائج += [عملية(عنصر)]
    نهاية
    ارجع نتائج
نهاية

دالة مربع(ن)
    ارجع ن * ن
نهاية

متغير أرقام = [1، 2، 3، 4، 5]
متغير مربعات = طبّق(أرقام، مربع)
اطبع(مربعات)  # [1، 4، 9، 16، 25]`,exercise:`# أنشئ دالة "رشّح" تأخذ قائمة ودالة شرط
# وتُرجع العناصر التي تحقق الشرط
دالة موجب(ن)
    ارجع ن > 0
نهاية

# أكمل دالة رشّح هنا
متغير بيانات = [-3، 5، -1، 8، 0، 4]
اطبع(رشّح(بيانات، موجب))`,hint:"أنشئ مصفوفة فارغة وكرر على القائمة وأضف العنصر إذا أرجعت دالة الشرط صحيح.",tips:["الدوال من الرتبة العليا تقبل دوالاً كمعاملات","تتيح فصل المنطق عن التكرار","أساس البرمجة الوظيفية"],concepts:["دوال الرتبة العليا","Higher-Order Functions","تمرير الدوال"]},{id:"l5_02",title:"النطاقات المغلقة (Closures)",desc:"الدوال التي تحتفظ بمتغيرات نطاقها الخارجي حتى بعد انتهاء تنفيذه.",explanation:'النطاقات المغلقة من أقوى المفاهيم في البرمجة الوظيفية — فهي تسمح للدالة بـ"تذكّر" البيئة التي أُنشئت فيها. تشبه الخزنة التي تحتفظ بمفتاحها حتى بعد خروج صاحبها من الغرفة. تُستخدم في إنشاء متغيرات خاصة (private state) بدون صنف، وفي مصانع الدوال (function factories)، وهي أساس أنماط مثل Module Pattern في JavaScript. فهمها ضروري لبناء أنظمة معقدة بحالة مغلفة.',code:`# النطاقات المغلقة — الدالة تحتفظ ببيئتها
دالة أنشئ_عدّاد(بداية)
    متغير قيمة = بداية
    ارجع لامدا()
        قيمة += 1
        ارجع قيمة
    نهاية
نهاية

متغير عدّاد1 = أنشئ_عدّاد(0)
متغير عدّاد2 = أنشئ_عدّاد(10)
اطبع(عدّاد1())  # 1
اطبع(عدّاد1())  # 2
اطبع(عدّاد2())  # 11`,exercise:`# أنشئ دالة تُرجع دالة ضرب بعدد ثابت
# مثال: ضارب_3 = أنشئ_ضارب(3)
# ضارب_3(5) → 15

# أكمل الدالة
دالة أنشئ_ضارب(عامل)
    # أرجع لامدا تضرب بالعامل
نهاية

متغير ضارب = أنشئ_ضارب(4)
اطبع(ضارب(7))  # 28`,hint:'أرجع لامدا تأخذ معاملاً واحداً وتضربه بـ "عامل" من النطاق الخارجي.',tips:["الـ Closure تحتفظ بمتغيرات النطاق الخارجي","كل استدعاء يُنشئ نطاقاً مغلقاً مستقلاً","مفيدة لإنشاء دوال مخصصة في وقت التشغيل"],concepts:["نطاق مغلق","Closure","بيئة معجمية"]},{id:"l5_03",title:"الكاري (Currying)",desc:"تحويل دالة متعددة المعاملات إلى سلسلة من دوال أحادية المعامل.",explanation:'الكاري تقنية قوية من الرياضيات اخترعها هاسكل كاري، تحوّل دالة f(a, b, c) إلى f(a)(b)(c). الفائدة الحقيقية تظهر عندما تعرف بعض المعاملات مبكراً وتريد إعادة استخدام الدالة. مثلاً، في نظام تسعير: إذا كانت الضريبة ثابتة 15%، تُنشئ دالة "حاسب_سعر_بضريبة" مرة واحدة وتستخدمها مع كل المنتجات. هذا يقلل تكرار الكود ويجعل التركيب (composition) أسهل.',code:`# الكاري — تحويل دالة ثنائية إلى سلسلة أحادية
دالة كاري_جمع(أ)
    ارجع لامدا(ب)
        ارجع أ + ب
    نهاية
نهاية

متغير أضف_خمسة = كاري_جمع(5)
اطبع(أضف_خمسة(3))   # 8
اطبع(أضف_خمسة(10))  # 15

# كاري ثلاثي
دالة كاري_ضرب_جمع(أ)
    ارجع لامدا(ب)
        ارجع لامدا(ج)
            ارجع أ * ب + ج
        نهاية
    نهاية
نهاية
اطبع(كاري_ضرب_جمع(2)(3)(4))  # 10`,exercise:`# أنشئ دالة كاري لحساب السعر بعد الخصم والضريبة
# كاري_سعر(خصم)(ضريبة)(سعر_أصلي)
# النتيجة = سعر_أصلي * (1 - خصم) * (1 + ضريبة)

دالة كاري_سعر(خصم)
    # أكمل الكاري هنا
نهاية

متغير حساب = كاري_سعر(0.1)(0.15)
اطبع(حساب(100))  # 103.5`,hint:"أرجع لامدا تأخذ الضريبة وتُرجع لامدا أخرى تأخذ السعر وتحسب النتيجة.",tips:["الكاري يُسهّل إنشاء دوال مخصصة جزئياً","كل خطوة تُنتج دالة جديدة بمعامل واحد","مفيد عندما نعرف بعض المعاملات مسبقاً"],concepts:["كاري","Currying","التطبيق الجزئي"]},{id:"l5_04",title:"تركيب الدوال (Composition)",desc:"بناء دوال معقدة بتركيب دوال بسيطة — خرج الأولى يدخل الثانية.",explanation:"تركيب الدوال مثل بناء الليجو: قطع صغيرة بسيطة تُجمع لتكوين هياكل معقدة. رياضياً: (f ∘ g)(x) = f(g(x)). في المشاريع الحقيقية، بدلاً من دالة ضخمة تفعل عشرة أشياء، تبني عشر دوال صغيرة وتركّبها. هذا يجعل كل دالة قابلة للاختبار منفردة، ويسهّل تعديل أي خطوة دون المساس بالباقي. أنماط مثل Middleware في Express.js مبنية على هذا المفهوم.",code:`# تركيب الدوال — دالة من دالتين
دالة ركّب(د1، د2)
    ارجع لامدا(س)
        ارجع د1(د2(س))
    نهاية
نهاية

دالة ضاعف(ن)
    ارجع ن * 2
نهاية

دالة زد_واحد(ن)
    ارجع ن + 1
نهاية

# ضاعف ثم زد واحد
متغير ضاعف_وزد = ركّب(زد_واحد، ضاعف)
اطبع(ضاعف_وزد(5))  # 11`,exercise:`# أنشئ دالة "سلسلة" تركّب قائمة من الدوال
# من اليمين لليسار (آخر دالة تُنفذ أولاً)
دالة سلسلة(دوال)
    # أكمل هنا
نهاية

متغير عملية = سلسلة([
    لامدا(ن) => ن + 1,
    لامدا(ن) => ن * 3,
    لامدا(ن) => ن - 2
])
اطبع(عملية(5))  # (5-2)*3+1 = 10`,hint:"كرر على الدوال من الأخيرة للأولى وطبق كل دالة على النتيجة التراكمية.",tips:["تركيب الدوال يبني عمليات معقدة من أجزاء بسيطة","الترتيب مهم: من اليمين لليسار رياضياً","يُسمى أيضاً function composition"],concepts:["تركيب الدوال","Function Composition","أنبوب البيانات"]},{id:"l5_05",title:"أنماط البيانات غير القابلة للتغيير",desc:"استخدام الثوابت والنسخ بدلاً من التعديل المباشر لتجنب الأخطاء.",explanation:"البيانات غير القابلة للتغيير (Immutability) مبدأ أساسي في هندسة البرمجيات الحديثة. تخيل أن كل مستند في مكتبك نسخة أصلية: بدلاً من الكتابة عليه، تأخذ صورة وتعدّل الصورة. هذا يضمن سلامة البيانات الأصلية، ويمنع الأخطاء الجانبية في البرامج متعددة الخيوط، ويُسهّل تتبع التغييرات (كما في Git). مكتبات مثل Redux وImmer في JavaScript مبنية بالكامل على هذا المفهوم.",code:`# أنماط عدم التغيير — نسخ بدلاً من تعديل
دالة أضف_عنصر(قائمة، عنصر)
    متغير جديدة = []
    لكل ع في قائمة
        جديدة += [ع]
    نهاية
    جديدة += [عنصر]
    ارجع جديدة
نهاية

ثابت أصلية = [1، 2، 3]
متغير محدّثة = أضف_عنصر(أصلية، 4)
اطبع(أصلية)   # [1، 2، 3] — لم تتغير
اطبع(محدّثة)  # [1، 2، 3، 4]`,exercise:`# أنشئ دالة "حدّث_خريطة" تُرجع نسخة من خريطة
# مع تغيير قيمة مفتاح محدد (بدون تعديل الأصل)
دالة حدّث_خريطة(خ، مفتاح، قيمة)
    # أنشئ خريطة جديدة وانسخ البيانات
    # ثم عدّل المفتاح المطلوب
نهاية

متغير شخص = {"اسم": "أحمد"، "عمر": 25}
متغير محدّث = حدّث_خريطة(شخص، "عمر"، 26)
اطبع(محدّث)`,hint:"أنشئ خريطة جديدة فارغة ثم انسخ كل أزواج المفاتيح والقيم إليها وعدّل المطلوب.",tips:["عدم تغيير البيانات الأصلية يمنع أخطاء جانبية","استخدم ثابت للقيم التي لا تتغير","أنشئ نسخاً جديدة بدلاً من التعديل المباشر"],concepts:["عدم التغيير","Immutability","نسخ البيانات"]},{id:"l5_06",title:"الأنماط التكرارية (Recursion)",desc:"حل المسائل بتقسيمها إلى حالات أصغر تستدعي نفسها حتى حالة الأساس.",explanation:"التكرار يشبه النظر في مرآتين متقابلتين: الدالة تستدعي نفسها بنسخة أصغر من المشكلة. في المشاريع الحقيقية، التكرار ضروري للتعامل مع البيانات الشجرية (ملفات، DOM، JSON متداخل) حيث لا تعرف العمق مسبقاً. المفتاح هو حالة الأساس التي توقف السلسلة. التكرار الذيلي (tail recursion) يُحسّن الأداء لأن المترجم يحوّله لحلقة داخلياً.",code:`# أنماط التكرار المتقدمة
دالة مسطّح(قائمة)
    متغير نتيجة = []
    لكل عنصر في قائمة
        إذا (نوع(عنصر) == "مصفوفة")
            متغير فرعي = مسطّح(عنصر)
            لكل ف في فرعي
                نتيجة += [ف]
            نهاية
        وإلا
            نتيجة += [عنصر]
        نهاية
    نهاية
    ارجع نتيجة
نهاية

متغير متداخل = [1، [2، [3، 4]]، 5]
اطبع(مسطّح(متداخل))  # [1، 2، 3، 4، 5]`,exercise:`# أنشئ دالة تكرارية "عمق" تحسب أقصى عمق لمصفوفة
# [1، 2] → 1
# [1، [2، 3]] → 2
# [1، [2، [3]]] → 3

دالة عمق(قائمة)
    # أكمل الدالة التكرارية
نهاية

اطبع(عمق([1، [2، [3]]]))  # 3`,hint:"الحالة الأساسية: ليست مصفوفة → ارجع 0. لكل عنصر مصفوفة احسب عمقه وأضف 1.",tips:["كل دالة تكرارية تحتاج حالة أساس لإيقاف التكرار","تأكد أن كل استدعاء يقترب من حالة الأساس","التكرار مثالي للبيانات الشجرية والمتداخلة"],concepts:["التكرار","Recursion","حالة الأساس"]},{id:"l5_07",title:"أنماط تعيين وترشيح وطي",desc:"بناء أنماط map و filter و reduce لمعالجة المجموعات بأسلوب وظيفي.",explanation:"هذه الثلاثية الذهبية (map/filter/reduce) هي أدوات المبرمج الوظيفي اليومية. Map يحوّل كل عنصر (تحويل الأسعار من دولار لريال). Filter يختار عناصر محددة (المنتجات المتوفرة فقط). Reduce يختزل القائمة لقيمة واحدة (إجمالي الفاتورة). في المشاريع الكبيرة، هذه الأنماط تحل 80% من عمليات معالجة البيانات بكود أنظف وأقل أخطاءً من الحلقات التقليدية.",code:`# تعيين وترشيح وطي (map/filter/reduce)
دالة تعيين(قائمة، دالة_تحويل)
    متغير نتيجة = []
    لكل ع في قائمة
        نتيجة += [دالة_تحويل(ع)]
    نهاية
    ارجع نتيجة
نهاية

دالة طي(قائمة، دالة_دمج، بداية)
    متغير مركم = بداية
    لكل ع في قائمة
        مركم = دالة_دمج(مركم، ع)
    نهاية
    ارجع مركم
نهاية

متغير أرقام = [1، 2، 3، 4، 5]
متغير مضاعفة = تعيين(أرقام، لامدا(ن) => ن * 2)
متغير مجموع = طي(أرقام، لامدا(م، ن) => م + ن، 0)
اطبع(مضاعفة)  # [2، 4، 6، 8، 10]
اطبع(مجموع)   # 15`,exercise:`# استخدم تعيين وطي لحساب متوسط مربعات الأرقام
# 1. عيّن كل رقم إلى مربعه
# 2. اطوِ المربعات لحساب المجموع
# 3. اقسم على عدد العناصر

متغير بيانات = [2، 4، 6، 8]

# أكمل الحل باستخدام الدوال الوظيفية
# النتيجة المتوقعة: 30.0`,hint:"عيّن كل رقم إلى ن*ن ثم اطوِ النتائج بالجمع واقسم على طول القائمة.",tips:["التعيين يحوّل كل عنصر بدون تغيير الحجم","الطي يختزل القائمة إلى قيمة واحدة","ادمج هذه الأنماط لبناء عمليات معقدة"],concepts:["تعيين","طي","Map/Filter/Reduce"]},{id:"l5_08",title:"التذكّر (Memoization)",desc:"تخزين نتائج الدوال السابقة لتجنب إعادة الحساب وتسريع الأداء.",explanation:'التذكّر هو تقنية تحسين الأداء الأقوى للدوال النقية: "إذا سُئلت نفس السؤال مرتين، أجب من الذاكرة". في فيبوناتشي، يحوّل O(2^n) إلى O(n). في المشاريع الحقيقية، يُستخدم لتخزين نتائج استعلامات API مكلفة، حسابات تشفير، أو أي عملية بطيئة مع نفس المدخلات. المكتبات مثل lodash.memoize وReact.memo تطبّق هذا النمط. التحدي: إدارة حجم الذاكرة المستخدمة.',code:`# التذكّر — تخزين نتائج محسوبة سابقاً
دالة أنشئ_متذكّر(دالة_حساب)
    متغير ذاكرة = {}
    ارجع لامدا(ن)
        متغير مفتاح = إلى_نص(ن)
        إذا (ذاكرة[مفتاح] != لاشيء)
            ارجع ذاكرة[مفتاح]
        نهاية
        متغير نتيجة = دالة_حساب(ن)
        ذاكرة[مفتاح] = نتيجة
        ارجع نتيجة
    نهاية
نهاية

دالة فيبوناتشي_بطيء(ن)
    إذا (ن <= 1)
        ارجع ن
    نهاية
    ارجع فيبوناتشي_بطيء(ن - 1) + فيبوناتشي_بطيء(ن - 2)
نهاية

متغير فيبو = أنشئ_متذكّر(فيبوناتشي_بطيء)
اطبع(فيبو(10))  # 55`,exercise:`# أنشئ دالة متذكّرة لحساب المضروب (factorial)
# مع طباعة رسالة عند الحساب الفعلي

دالة أنشئ_مضروب_متذكّر()
    متغير ذاكرة = {}
    ارجع لامدا(ن)
        # أكمل: تحقق من الذاكرة أولاً
        # ثم احسب وخزّن النتيجة
    نهاية
نهاية

متغير مضروب = أنشئ_مضروب_متذكّر()
اطبع(مضروب(5))  # 120`,hint:"حوّل ن لنص كمفتاح، تحقق إن كان في الذاكرة، وإلا احسبه تكرارياً وخزّنه.",tips:["التذكّر يُحسّن أداء الدوال المكلفة حسابياً","استخدم خريطة لتخزين النتائج بمفتاح المدخلات","مثالي لفيبوناتشي والمضروب والبرمجة الديناميكية"],concepts:["التذكّر","Memoization","التخزين المؤقت"]},{id:"l5_09",title:"نمط الأنبوب (Pipeline)",desc:"تمرير البيانات عبر سلسلة من الدوال بالتتابع لمعالجة متدرجة.",explanation:"نمط الأنبوب مثل خط إنتاج المصنع: البيانات تدخل من طرف وتمر بمحطات معالجة متتالية حتى تخرج منتهية. في Unix، الأوامر تتصل بـ | لهذا الغرض. في البرمجة، هذا النمط يجعل تدفق البيانات واضحاً وقابلاً للتعديل. إذا أردت إضافة خطوة تحقق، تضيفها في السلسلة دون تعديل الخطوات الأخرى. أطر مثل RxJS وNode.js Streams مبنية على هذا المفهوم.",code:`# نمط الأنبوب — سلسلة معالجة متتابعة
دالة أنبوب(قيمة، دوال)
    متغير نتيجة = قيمة
    لكل دالة_حالية في دوال
        نتيجة = دالة_حالية(نتيجة)
    نهاية
    ارجع نتيجة
نهاية

# سلسلة معالجة نص
متغير خطوات = [
    لامدا(ن) => ن * 2,
    لامدا(ن) => ن + 10,
    لامدا(ن) => ن * 3
]

اطبع(أنبوب(5، خطوات))  # (5*2+10)*3 = 60`,exercise:`# أنشئ أنبوب معالجة لمصفوفة أرقام:
# 1. رشّح الأرقام الزوجية فقط
# 2. ضاعف كل رقم
# 3. احسب المجموع

دالة أنبوب(قيمة، دوال)
    متغير نتيجة = قيمة
    لكل د في دوال
        نتيجة = د(نتيجة)
    نهاية
    ارجع نتيجة
نهاية

متغير بيانات = [1، 2، 3، 4، 5، 6]
# أكمل خطوات الأنبوب`,hint:"اكتب ثلاث لامدا: الأولى ترشّح الزوجية، الثانية تعيّن بالضرب، الثالثة تطوي بالجمع.",tips:["الأنبوب يجعل تدفق البيانات واضحاً وخطياً","كل خطوة دالة نقية بمدخل ومخرج","سهل الاختبار والتعديل بإضافة أو حذف خطوات"],concepts:["نمط الأنبوب","Pipeline","سلسلة المعالجة"]},{id:"l5_10",title:"معالجة الأخطاء الوظيفية",desc:"استخدام الأنماط الوظيفية لمعالجة الأخطاء بدلاً من الاستثناءات.",explanation:"الاستثناءات تكسر تدفق البرنامج وتجعل تتبع الأخطاء صعباً. الحل الوظيفي: نوع Result يغلّف إما النجاح أو الفشل صراحةً. هذا يُجبر المبرمج على معالجة الأخطاء، ويتيح سلسلة العمليات مع معالجة تلقائية للفشل. لغات مثل Rust وSwift تبنّت هذا النمط. في المشاريع الكبيرة، يجعل الكود أكثر استقراراً لأن كل خطأ محتمل مرئي وموثق في نوع الإرجاع.",code:`# معالجة أخطاء وظيفية — نتيجة بدلاً من استثناء
صنف نتيجة
    باني(نجاح، قيمة، خطأ)
        هذا.نجاح = نجاح
        هذا.قيمة = قيمة
        هذا.خطأ = خطأ
    نهاية
نهاية

دالة نجاح(قيمة)
    ارجع جديد نتيجة(صحيح، قيمة، لاشيء)
نهاية

دالة فشل(رسالة)
    ارجع جديد نتيجة(خطأ، لاشيء، رسالة)
نهاية

دالة اقسم(أ، ب)
    إذا (ب == 0)
        ارجع فشل("القسمة على صفر")
    نهاية
    ارجع نجاح(أ / ب)
نهاية

متغير ن1 = اقسم(10، 2)
إذا (ن1.نجاح)
    اطبع(ن1.قيمة)  # 5
نهاية`,exercise:`# أنشئ دالة "سلسلة_نتائج" تأخذ نتيجة ودالة
# إذا كانت النتيجة ناجحة طبّق الدالة
# وإلا أرجع الخطأ كما هو

دالة سلسلة_نتائج(نتيجة، دالة_تحويل)
    # أكمل هنا
نهاية

# اختبر: اقسم ثم اضرب في 3
متغير ن = اقسم(10، 2)
متغير م = سلسلة_نتائج(ن، لامدا(ق) => نجاح(ق * 3))
اطبع(م.قيمة)  # 15`,hint:"تحقق من نتيجة.نجاح، إذا صحيح طبّق الدالة على نتيجة.قيمة، وإلا أرجع النتيجة.",tips:["النتيجة تغلّف النجاح والفشل في نوع واحد","السلسلة تتيح ربط عمليات مع معالجة أخطاء تلقائية","هذا النمط يُسمى Result/Either في لغات أخرى"],concepts:["نتيجة وظيفية","Result Pattern","معالجة أخطاء"]}]},Tc={id:"l5_data_structures",title:"هياكل البيانات المخصصة",icon:"🏗️",lessons:[{id:"l5_11",title:"القائمة المترابطة (Linked List)",desc:"بناء قائمة مترابطة من عُقد يشير كل منها للعقدة التالية.",explanation:"القائمة المترابطة مثل قطار حيث كل عربة مرتبطة بالتالية فقط. على عكس المصفوفة التي تخزّن العناصر متصلة في الذاكرة، كل عقدة مستقلة وتحمل عنوان التالية. هذا يجعل الإدراج والحذف في الوسط O(1) بدلاً من O(n). تُستخدم في تنفيذ المكدسات والطوابير، وفي إدارة ذاكرة نظام التشغيل، وفي تطبيقات مثل قوائم التشغيل (playlists) وسجلات التراجع (undo).",code:`# القائمة المترابطة
صنف عقدة
    باني(قيمة)
        هذا.قيمة = قيمة
        هذا.التالي = لاشيء
    نهاية
نهاية

صنف قائمة_مترابطة
    باني()
        هذا.الرأس = لاشيء
    نهاية
    دالة أضف(قيمة)
        متغير ج = جديد عقدة(قيمة)
        ج.التالي = هذا.الرأس
        هذا.الرأس = ج
    نهاية
نهاية

متغير ق = جديد قائمة_مترابطة()
ق.أضف(3)
ق.أضف(2)
ق.أضف(1)`,exercise:`# أضف دالة "ابحث" تُرجع صحيح إذا وُجدت القيمة
صنف قائمة يرث قائمة_مترابطة
    باني()
        هذا.الرأس = لاشيء
    نهاية
    دالة ابحث(قيمة)
        # تنقل من الرأس عبر التالي
        # أرجع صحيح عند الإيجاد
    نهاية
نهاية`,hint:"ابدأ من الرأس وتنقل عبر التالي حتى تجد القيمة أو تصل للاشيء.",tips:["القائمة المترابطة تتيح إضافة وحذف سريع","كل عقدة تحتوي قيمة ومؤشر للتالية","لا تدعم الوصول العشوائي مثل المصفوفات"],concepts:["قائمة مترابطة","Linked List","عقدة"]},{id:"l5_12",title:"المكدّس (Stack)",desc:"بنية بيانات تعمل بمبدأ الداخل أخيراً يخرج أولاً (LIFO).",explanation:"المكدس مثل كومة أطباق: تضع الجديد فوق وتأخذ من الأعلى. في الحوسبة، المكدس أساسي: استدعاءات الدوال تُخزّن على مكدس التنفيذ (call stack)، وزر الرجوع في المتصفح يستخدم مكدساً. خطأ Stack Overflow الشهير يحدث عند امتلائه. يُستخدم أيضاً في تحليل الأقواس، وتحويل التعابير الحسابية، والتراجع (undo) في التطبيقات.",code:`# المكدّس — LIFO
صنف مكدّس
    باني()
        هذا.عناصر = []
    نهاية
    دالة ادفع(قيمة)
        هذا.عناصر += [قيمة]
    نهاية
    دالة انزع()
        إذا (طول(هذا.عناصر) == 0)
            ارمي "المكدّس فارغ"
        نهاية
        ارجع هذا.عناصر[طول(هذا.عناصر) - 1]
    نهاية
نهاية

متغير م = جديد مكدّس()
م.ادفع(10)
م.ادفع(20)
اطبع(م.انزع())  # 20`,exercise:`# استخدم المكدّس لعكس مصفوفة
صنف مكدّس
    باني()
        هذا.عناصر = []
    نهاية
    # أضف ادفع وانزع
نهاية

دالة اعكس(قائمة)
    # ادفع كل العناصر ثم انزعها
نهاية
اطبع(اعكس([1، 2، 3]))  # [3، 2، 1]`,hint:"ادفع كل عنصر ثم انزعهم في مصفوفة جديدة.",tips:["المكدّس LIFO — آخر داخل أول خارج","مفيد لعكس البيانات وتتبع العمليات","يُستخدم في تنفيذ استدعاءات الدوال"],concepts:["مكدّس","Stack","LIFO"]},{id:"l5_13",title:"الطابور (Queue)",desc:"بنية بيانات تعمل بمبدأ الداخل أولاً يخرج أولاً (FIFO).",explanation:"الطابور مثل طابور البنك: من يصل أولاً يُخدَم أولاً. في الأنظمة الحقيقية، الطوابير في كل مكان: طابور الطباعة، طابور الرسائل (Message Queue مثل RabbitMQ)، طابور الأحداث في المتصفح (Event Loop). طابور الأولويات متغير يُخرج الأهم أولاً بدلاً من الأقدم، ويُستخدم في جدولة المهام وخوارزميات الرسوم البيانية.",code:`# الطابور — FIFO
صنف طابور
    باني()
        هذا.عناصر = []
    نهاية
    دالة أدخل(قيمة)
        هذا.عناصر += [قيمة]
    نهاية
    دالة أخرج()
        إذا (طول(هذا.عناصر) == 0)
            ارمي "الطابور فارغ"
        نهاية
        متغير أول = هذا.عناصر[0]
        متغير جديد = []
        لكل ي في نطاق(1، طول(هذا.عناصر))
            جديد += [هذا.عناصر[ي]]
        نهاية
        هذا.عناصر = جديد
        ارجع أول
    نهاية
نهاية

متغير ط = جديد طابور()
ط.أدخل("أحمد")
ط.أدخل("سارة")
اطبع(ط.أخرج())  # أحمد`,exercise:`# أنشئ طابور بأولويات
صنف طابور_أولويات
    باني()
        هذا.عناصر = []
    نهاية
    دالة أدخل(اسم، أولوية)
        # أضف حسب الأولوية
    نهاية
    دالة أخرج()
        # أخرج الأعلى أولوية
    نهاية
نهاية`,hint:"رقم أقل = أولوية أعلى. ابحث عن الأصغر عند الإخراج.",tips:["الطابور FIFO — أول داخل أول خارج","مفيد لمعالجة الطلبات بالترتيب","طابور الأولوية يُخرج الأهم أولاً"],concepts:["طابور","Queue","FIFO"]},{id:"l5_14",title:"الشجرة الثنائية (Binary Tree)",desc:"بنية شجرية يحتوي كل عنصر فيها على فرعين: أيسر وأيمن.",explanation:"الشجرة الثنائية من أهم هياكل البيانات في علوم الحاسوب. شجرة البحث الثنائي (BST) ترتب الأصغر يساراً والأكبر يميناً، مما يجعل البحث O(log n). تُستخدم في قواعد البيانات (فهارس B-Tree)، وفي الفرز (heap sort)، وفي ضغط البيانات (Huffman trees). DOM في المتصفح شجرة، وملفات النظام شجرة. فهم التجول الشجري (داخلي، قبلي، بعدي) ضروري للمقابلات.",code:`# شجرة بحث ثنائية
صنف عقدة_شجرة
    باني(قيمة)
        هذا.قيمة = قيمة
        هذا.يسار = لاشيء
        هذا.يمين = لاشيء
    نهاية
نهاية

دالة أدخل(عقدة، قيمة)
    إذا (عقدة == لاشيء)
        ارجع جديد عقدة_شجرة(قيمة)
    نهاية
    إذا (قيمة < عقدة.قيمة)
        عقدة.يسار = أدخل(عقدة.يسار، قيمة)
    وإلا
        عقدة.يمين = أدخل(عقدة.يمين، قيمة)
    نهاية
    ارجع عقدة
نهاية

متغير جذر = لاشيء
جذر = أدخل(جذر، 5)
جذر = أدخل(جذر، 3)
جذر = أدخل(جذر، 7)`,exercise:`# أضف دالة بحث تكرارية في الشجرة
دالة ابحث(عقدة، قيمة)
    # لاشيء → خطأ
    # تساوي → صحيح
    # أصغر → يسار، أكبر → يمين
نهاية

متغير ج = أدخل(لاشيء، 5)
ج = أدخل(ج، 3)
ج = أدخل(ج، 7)
اطبع(ابحث(ج، 3))  # صحيح`,hint:"إذا العقدة لاشيء ارجع خطأ. تساوت؟ صحيح. وإلا ابحث يسار أو يمين.",tips:["يسار أقل، يمين أكبر في شجرة البحث","البحث بتعقيد O(log n) في الحالة المثلى","التجول الداخلي يعطي البيانات مرتبة"],concepts:["شجرة ثنائية","Binary Tree","بحث شجري"]},{id:"l5_15",title:"جدول التجزئة (Hash Table)",desc:"ربط المفاتيح بالقيم عبر دالة تجزئة للوصول السريع.",explanation:"جدول التجزئة مثل الفهرس الأبجدي في الكتاب: تحوّل المفتاح لرقم مباشرة بدلاً من البحث التسلسلي. الوصول O(1) في المتوسط. التحدي: التصادمات عندما ينتج مفتاحان نفس الفهرس، وتُحل بالسلاسل (chaining) أو العنونة المفتوحة. كل لغة برمجة تستخدمها: Dict في Python، Object/Map في JavaScript، HashMap في Java. فهم تفاصيلها ضروري لمعرفة متى تتدهور الأداء.",code:`# جدول تجزئة بسيط
صنف جدول_تجزئة
    باني(حجم)
        هذا.حجم = حجم
        هذا.سلال = []
        لكل ي في نطاق(0، حجم)
            هذا.سلال += [[]]
        نهاية
    نهاية
    دالة _تجزئة(مفتاح)
        ارجع طول(مفتاح) % هذا.حجم
    نهاية
    دالة ضع(مفتاح، قيمة)
        متغير ف = هذا._تجزئة(مفتاح)
        هذا.سلال[ف] += [[مفتاح، قيمة]]
    نهاية
    دالة اقرأ(مفتاح)
        متغير ف = هذا._تجزئة(مفتاح)
        لكل زوج في هذا.سلال[ف]
            إذا (زوج[0] == مفتاح)
                ارجع زوج[1]
            نهاية
        نهاية
        ارجع لاشيء
    نهاية
نهاية

متغير ج = جديد جدول_تجزئة(10)
ج.ضع("اسم"، "أحمد")
اطبع(ج.اقرأ("اسم"))  # أحمد`,exercise:`# أضف دالة "احذف" لجدول التجزئة
دالة احذف(جدول، مفتاح)
    # ابحث في السلة المناسبة
    # أعد بناءها بدون الزوج المحذوف
نهاية

متغير ج = جديد جدول_تجزئة(10)
ج.ضع("أ"، 1)
احذف(ج، "أ")
اطبع(ج.اقرأ("أ"))  # لاشيء`,hint:"احسب فهرس التجزئة وأعد بناء السلة بدون الزوج المطلوب حذفه.",tips:["وصول O(1) في المتوسط","التصادمات تُحل بالسلاسل (chaining)","دالة التجزئة الجيدة توزع بالتساوي"],concepts:["جدول تجزئة","Hash Table","دالة تجزئة"]},{id:"l5_16",title:"طابور الأولوية (Priority Queue)",desc:"طابور يُخرج العنصر ذا الأولوية الأعلى بدلاً من الأقدم.",explanation:"طابور الأولوية مثل غرفة الطوارئ في المستشفى: الحالة الأخطر تُعالج أولاً بغض النظر عن وقت الوصول. التنفيذ الأمثل بـ Heap يعطي O(log n) للإدراج والإخراج. يُستخدم في: جدولة أنظمة التشغيل، خوارزمية Dijkstra لأقصر مسار، معالجة الطلبات العاجلة في الخوادم، وضغط Huffman.",code:`# طابور أولوية
صنف طابور_أولوية
    باني()
        هذا.عناصر = []
    نهاية
    دالة أدخل(قيمة، أولوية)
        هذا.عناصر += [{"قيمة": قيمة، "أولوية": أولوية}]
    نهاية
    دالة أخرج()
        متغير أفضل = 0
        لكل ي في نطاق(1، طول(هذا.عناصر))
            إذا (هذا.عناصر[ي]["أولوية"] < هذا.عناصر[أفضل]["أولوية"])
                أفضل = ي
            نهاية
        نهاية
        ارجع هذا.عناصر[أفضل]["قيمة"]
    نهاية
نهاية

متغير ط = جديد طابور_أولوية()
ط.أدخل("عادية"، 3)
ط.أدخل("عاجلة"، 1)
اطبع(ط.أخرج())  # عاجلة`,exercise:`# أنشئ نظام مهام بأولويات وحالات
صنف نظام_مهام
    باني()
        هذا.مهام = []
    نهاية
    دالة أضف(اسم، أولوية)
        # مهمة: {اسم، أولوية، حالة: "جديد"}
    نهاية
    دالة التالية()
        # أرجع الأعلى أولوية
    نهاية
نهاية`,hint:"خزّن كل مهمة كخريطة. ابحث عن الأقل رقم أولوية.",tips:["يُخرج الأهم لا الأقدم","التنفيذ بمصفوفة بسيط","يُستخدم في جدولة المهام"],concepts:["طابور أولوية","Priority Queue","جدولة"]},{id:"l5_17",title:"أساسيات الرسم البياني (Graph)",desc:"تمثيل العلاقات بين العناصر باستخدام العُقد والحواف.",explanation:"الرسوم البيانية من أقوى هياكل البيانات لتمثيل العلاقات. شبكة الإنترنت رسم بياني، Facebook رسم بياني للأصدقاء، خرائط Google رسم بياني للطرق. التمثيل بقائمة الجوار كفء للرسوم المتناثرة، وبمصفوفة للكثيفة. خوارزميات BFS وDFS أساسية لحل مسائل مثل أقصر مسار واكتشاف الشبكات.",code:`# رسم بياني بقائمة جوار
صنف رسم_بياني
    باني()
        هذا.جوار = {}
    نهاية
    دالة أضف_عقدة(اسم)
        إذا (هذا.جوار[اسم] == لاشيء)
            هذا.جوار[اسم] = []
        نهاية
    نهاية
    دالة أضف_حافة(من، إلى)
        هذا.جوار[من] += [إلى]
        هذا.جوار[إلى] += [من]
    نهاية
نهاية

متغير ر = جديد رسم_بياني()
ر.أضف_عقدة("أ")
ر.أضف_عقدة("ب")
ر.أضف_عقدة("ج")
ر.أضف_حافة("أ"، "ب")
ر.أضف_حافة("ب"، "ج")
اطبع(ر.جوار["ب"])  # [أ، ج]`,exercise:`# أنشئ دالة BFS للبحث بالعرض
دالة بحث_عرض(رسم، بداية)
    متغير زُرت = {}
    متغير طابور = [بداية]
    # أخرج من المقدمة
    # إن لم تُزر: سجّلها وأضف جيرانها
نهاية

متغير ر = جديد رسم_بياني()
# أضف عقد وحواف واختبر`,hint:"كرر ما دام الطابور غير فارغ. أخرج عقدة وأضف جيرانها.",tips:["الرسم البياني يمثل العلاقات","قائمة الجوار كفؤة للرسوم المتناثرة","BFS بطابور، DFS بمكدّس"],concepts:["رسم بياني","Graph","بحث بالعرض"]},{id:"l5_18",title:"تنفيذ المجموعة (Set)",desc:"بنية لا تسمح بالتكرار مع عمليات اتحاد وتقاطع.",explanation:"المجموعة مفهوم رياضي: مجموعة عناصر فريدة بدون ترتيب. في البرمجة، ضرورية لإزالة التكرارات، وفحص العضوية بسرعة O(1). عمليات الاتحاد والتقاطع والفرق مفيدة في: تحليل الصلاحيات (من يملك كلا الإذنين؟)، ومقارنة قوائم (المتابعون المشتركون)، وتتبع العناصر المزارة في الخوارزميات.",code:`# مجموعة — بدون تكرار
صنف مجموعة
    باني()
        هذا.عناصر = []
    نهاية
    دالة أضف(قيمة)
        لكل ع في هذا.عناصر
            إذا (ع == قيمة)
                ارجع خطأ
            نهاية
        نهاية
        هذا.عناصر += [قيمة]
        ارجع صحيح
    نهاية
    دالة اتحاد(أخرى)
        متغير ن = جديد مجموعة()
        لكل ع في هذا.عناصر
            ن.أضف(ع)
        نهاية
        لكل ع في أخرى.عناصر
            ن.أضف(ع)
        نهاية
        ارجع ن
    نهاية
نهاية

متغير م1 = جديد مجموعة()
م1.أضف(1)
م1.أضف(2)
م1.أضف(3)`,exercise:`# أضف دالة "تقاطع" تُرجع العناصر المشتركة
صنف مجموعة_متقدمة يرث مجموعة
    باني()
        الأساس()
    نهاية
    دالة تقاطع(أخرى)
        # كرر على عناصرك
        # أضف المشترك فقط لمجموعة جديدة
    نهاية
نهاية`,hint:"كرر على عناصرك وتحقق إن كان كل عنصر في الأخرى.",tips:["المجموعة ترفض التكرار تلقائياً","الاتحاد والتقاطع عمليات أساسية","التنفيذ الكفء بجدول تجزئة"],concepts:["مجموعة","Set","عمليات المجموعات"]},{id:"l5_19",title:"المخزن الدائري (Circular Buffer)",desc:"مصفوفة بحجم ثابت تلتف من النهاية للبداية.",explanation:"المخزن الدائري مثل كاميرا المراقبة التي تسجل آخر 24 ساعة فقط: عند الامتلاء، تبدأ الكتابة فوق الأقدم. المفتاح: عامل المتبقي (%) لحساب الفهرس الدائري. يُستخدم في: تخزين آخر N سجل (logs)، وذاكرة الصوت/الفيديو المؤقتة (streaming buffers)، وأنظمة الزمن الحقيقي. الميزة: حجم ثابت مهما طال التشغيل.",code:`# المخزن الدائري
صنف مخزن_دائري
    باني(سعة)
        هذا.سعة = سعة
        هذا.بيانات = []
        لكل ي في نطاق(0، سعة)
            هذا.بيانات += [لاشيء]
        نهاية
        هذا.فهرس = 0
        هذا.عدد = 0
    نهاية
    دالة اكتب(قيمة)
        هذا.بيانات[هذا.فهرس] = قيمة
        هذا.فهرس = (هذا.فهرس + 1) % هذا.سعة
        إذا (هذا.عدد < هذا.سعة)
            هذا.عدد += 1
        نهاية
    نهاية
نهاية

متغير م = جديد مخزن_دائري(3)
م.اكتب(1)
م.اكتب(2)
م.اكتب(3)
م.اكتب(4)  # يكتب فوق 1`,exercise:`# أنشئ سجل رسائل بسعة ثابتة
صنف سجل
    باني(سعة)
        هذا.سعة = سعة
        هذا.رسائل = []
        هذا.فهرس = 0
    نهاية
    دالة سجّل(رسالة)
        # اكتب في الموضع الدائري
    نهاية
    دالة اعرض()
        # اعرض الرسائل
    نهاية
نهاية`,hint:"استخدم عامل المتبقي (%) لحساب الفهرس الدائري.",tips:["حجم ثابت يلتف تلقائياً","مثالي لآخر N عنصر","عامل % هو مفتاح الالتفاف"],concepts:["مخزن دائري","Circular Buffer","Ring Buffer"]},{id:"l5_20",title:"اختيار هيكل البيانات المناسب",desc:"مقارنة هياكل البيانات واختيار الأنسب حسب المتطلبات.",explanation:"اختيار هيكل البيانات قرار هندسي حاسم: الاختيار الخاطئ يحوّل التطبيق السريع إلى بطيء. القاعدة: حلّل العمليات الأكثر تكراراً. إذا كان البحث الأكثر، استخدم خريطة. إذا كان الترتيب مهماً، استخدم شجرة. إذا كان الإدراج/الحذف كثيراً، استخدم قائمة مترابطة. المحترف يعرف المقايضات (trade-offs) لكل خيار.",code:`# مقارنة هياكل البيانات
# مصفوفة — وصول بالفهرس O(1)
متغير أسماء = ["أحمد"، "سارة"، "خالد"]
اطبع(أسماء[1])

# خريطة — بحث بالمفتاح O(1)
متغير هاتف = {"أحمد": "123"، "سارة": "456"}
اطبع(هاتف["أحمد"])

# اختيار حسب العملية
دالة اقترح(عملية)
    طابق (عملية)
        عندما "بحث": ارجع "خريطة"
        عندما "ترتيب": ارجع "شجرة"
        عندما "تراجع": ارجع "مكدّس"
        افتراضي: ارجع "مصفوفة"
    نهاية
نهاية
اطبع(اقترح("بحث"))`,exercise:`# صمم نظام مكتبة بالهياكل المناسبة
صنف مكتبة
    باني()
        هذا.كتب = {}
        هذا.انتظار = []
    نهاية
    دالة أضف_كتاب(رقم، عنوان)
        # خزّن بالخريطة
    نهاية
    دالة استعر(رقم، شخص)
        # تحقق وسجّل
    نهاية
نهاية`,hint:"خريطة للوصول السريع، مصفوفة للترتيب الزمني.",tips:["مصفوفة للوصول المفهرس","خريطة للبحث بالمفتاح","اختر حسب العملية الأكثر تكراراً"],concepts:["اختيار هيكل بيانات","تعقيد زمني","مقايضات"]}]},Oc={id:"l5_algorithms",title:"الخوارزميات وحل المسائل",icon:"⚡",lessons:[{id:"l5_21",title:"الترتيب الفقاعي (Bubble Sort)",desc:"خوارزمية ترتيب بسيطة تقارن العناصر المتجاورة وتبدّلها.",explanation:'الترتيب الفقاعي سُمي بذلك لأن العناصر الكبيرة "تطفو" للأعلى كالفقاعات. بسيط للفهم لكنه O(n²) — بطيء جداً للبيانات الكبيرة. في الواقع، لا يُستخدم في الإنتاج إلا لقوائم صغيرة جداً أو شبه مرتبة. لكنه مهم تعليمياً لفهم مبادئ الترتيب وتحليل التعقيد. التحسين: إيقافه مبكراً إذا لم يحدث تبديل في الجولة.',code:`# الترتيب الفقاعي
دالة ترتيب_فقاعي(مصفوفة)
    متغير ن = طول(مصفوفة)
    لكل ي في نطاق(0، ن)
        لكل ج في نطاق(0، ن - ي - 1)
            إذا (مصفوفة[ج] > مصفوفة[ج + 1])
                متغير م = مصفوفة[ج]
                مصفوفة[ج] = مصفوفة[ج + 1]
                مصفوفة[ج + 1] = م
            نهاية
        نهاية
    نهاية
    ارجع مصفوفة
نهاية

اطبع(ترتيب_فقاعي([64، 34، 25، 12، 22]))`,exercise:`# حسّن الفقاعي بإيقافه إذا لم يحدث تبديل
دالة ترتيب_محسّن(مصفوفة)
    متغير ن = طول(مصفوفة)
    لكل ي في نطاق(0، ن)
        متغير تبديل = خطأ
        # أكمل: الحلقة الداخلية
        # إذا تبديل == خطأ → توقف
    نهاية
    ارجع مصفوفة
نهاية

اطبع(ترتيب_محسّن([5، 1، 4، 2، 8]))`,hint:'أضف متغير "تبديل" يبدأ بـ خطأ. عند كل تبديل اجعله صحيح. إذا بقي خطأ توقف.',tips:["الترتيب الفقاعي O(n²) — بطيء لكن بسيط","الإيقاف المبكر يفيد القوائم شبه المرتبة","مفيد تعليمياً لفهم مبادئ الترتيب"],concepts:["ترتيب فقاعي","Bubble Sort","O(n²)"]},{id:"l5_22",title:"ترتيب الاختيار والإدراج",desc:"خوارزميتا ترتيب: اختيار الأصغر وإدراج في المكان الصحيح.",explanation:'ترتيب الاختيار يبحث عن الأصغر ويضعه في مكانه، مثل ترتيب الأوراق بالبحث عن الأقل رقماً. ترتيب الإدراج يبني القائمة المرتبة عنصراً بعنصر، مثل ترتيب ورق اللعب في يدك. كلاهما O(n²) لكن الإدراج أفضل للقوائم شبه المرتبة ويعمل "online" (يرتب عند وصول كل عنصر). مهم للمقابلات.',code:`# ترتيب الاختيار — اختر الأصغر
دالة ترتيب_اختيار(مصفوفة)
    متغير ن = طول(مصفوفة)
    لكل ي في نطاق(0، ن)
        متغير أصغر = ي
        لكل ج في نطاق(ي + 1، ن)
            إذا (مصفوفة[ج] < مصفوفة[أصغر])
                أصغر = ج
            نهاية
        نهاية
        متغير م = مصفوفة[ي]
        مصفوفة[ي] = مصفوفة[أصغر]
        مصفوفة[أصغر] = م
    نهاية
    ارجع مصفوفة
نهاية

اطبع(ترتيب_اختيار([29، 10، 14، 37، 13]))`,exercise:`# نفّذ ترتيب الإدراج
دالة ترتيب_إدراج(مصفوفة)
    متغير ن = طول(مصفوفة)
    لكل ي في نطاق(1، ن)
        متغير مفتاح = مصفوفة[ي]
        متغير ج = ي - 1
        # أكمل: انقل الأكبر يميناً
        # أدرج المفتاح في مكانه
    نهاية
    ارجع مصفوفة
نهاية

اطبع(ترتيب_إدراج([12، 11، 13، 5، 6]))`,hint:"بينما ج >= 0 والمفتاح أصغر، انقل [ج] لـ [ج+1] وأنقص ج.",tips:["الاختيار يجد الأصغر ويضعه في مكانه","الإدراج يدرج كل عنصر بالجزء المرتب","الإدراج أفضل للمصفوفات شبه المرتبة"],concepts:["ترتيب الاختيار","ترتيب الإدراج","Selection Sort"]},{id:"l5_23",title:"الترتيب بالدمج (Merge Sort)",desc:"ترتيب فعّال يقسم المصفوفة وتدمج الأجزاء المرتبة.",explanation:"الترتيب بالدمج مثال كلاسيكي لـ Divide and Conquer: قسّم المشكلة لأجزاء، حل كل جزء، ادمج الحلول. تعقيد O(n log n) مضمون (ليس عشوائي مثل Quick Sort). مستقر (stable): العناصر المتساوية تحافظ على ترتيبها الأصلي. العيب: يحتاج O(n) ذاكرة إضافية. يُستخدم في ترتيب الملفات الضخمة وترتيب القوائم المترابطة.",code:`# دالة الدمج
دالة ادمج(يسار، يمين)
    متغير نتيجة = []
    متغير ي = 0
    متغير م = 0
    بينما (ي < طول(يسار) و م < طول(يمين))
        إذا (يسار[ي] <= يمين[م])
            نتيجة += [يسار[ي]]
            ي += 1
        وإلا
            نتيجة += [يمين[م]]
            م += 1
        نهاية
    نهاية
    بينما (ي < طول(يسار))
        نتيجة += [يسار[ي]]
        ي += 1
    نهاية
    بينما (م < طول(يمين))
        نتيجة += [يمين[م]]
        م += 1
    نهاية
    ارجع نتيجة
نهاية

اطبع(ادمج([1، 3، 5]، [2، 4، 6]))`,exercise:`# نفّذ دالة دمج مصفوفتين مرتبتين
دالة ادمج_مرتبتين(أ، ب)
    متغير نتيجة = []
    متغير ي = 0
    متغير م = 0
    # أكمل: قارن وأضف الأصغر
    # ثم أضف المتبقي
    ارجع نتيجة
نهاية

اطبع(ادمج_مرتبتين([1، 3، 5]، [2، 4، 6]))`,hint:"استخدم مؤشرَين وقارن. أضف الأصغر وحرّك مؤشره.",tips:["الترتيب بالدمج O(n log n) — فعّال ومستقر","يستخدم فرّق تسد (Divide & Conquer)","يحتاج ذاكرة إضافية O(n)"],concepts:["ترتيب بالدمج","Merge Sort","فرّق تسد"]},{id:"l5_24",title:"البحث الثنائي (Binary Search)",desc:"بحث فعّال في مصفوفة مرتبة بتقسيم مجال البحث للنصف.",explanation:"البحث الثنائي مثل البحث في قاموس: تفتح المنتصف، إذا الكلمة أبكر تذهب يساراً، وإلا يميناً. هذا يحوّل O(n) إلى O(log n) — الفرق هائل: لمليون عنصر، 20 مقارنة بدلاً من مليون. شرطه: المصفوفة مرتبة. يُستخدم في: فهارس قواعد البيانات، البحث في APIs، وإيجاد الحدود. من أهم خوارزميات المقابلات.",code:`# البحث الثنائي — O(log n)
دالة بحث_ثنائي(مصفوفة، هدف)
    متغير يسار = 0
    متغير يمين = طول(مصفوفة) - 1
    بينما (يسار <= يمين)
        متغير وسط = (يسار + يمين) / 2
        إذا (مصفوفة[وسط] == هدف)
            ارجع وسط
        وإلا
            إذا (مصفوفة[وسط] < هدف)
                يسار = وسط + 1
            وإلا
                يمين = وسط - 1
            نهاية
        نهاية
    نهاية
    ارجع -1
نهاية

متغير أ = [2، 5، 8، 12، 16، 23، 38]
اطبع(بحث_ثنائي(أ، 23))  # 5`,exercise:`# نفّذ بحث ثنائي تكراري (recursive)
دالة بحث_تكراري(مصفوفة، هدف، يسار، يمين)
    # حالة الأساس: يسار > يمين → -1
    # الوسط == الهدف → الوسط
    # أصغر → ابحث يمين، أكبر → ابحث يسار
نهاية

متغير أ = [1، 3، 5، 7، 9، 11]
اطبع(بحث_تكراري(أ، 7، 0، 5))  # 3`,hint:"حالة الأساس: يسار > يمين. احسب الوسط واستدعِ نفسك على النصف المناسب.",tips:["يتطلب مصفوفة مرتبة مسبقاً","يقسم المجال للنصف كل مرة: O(log n)","أسرع بكثير من البحث الخطي"],concepts:["بحث ثنائي","Binary Search","O(log n)"]},{id:"l5_25",title:"إتقان التكرار (Recursion)",desc:"تقنيات التكرار المتقدمة: الذيلي والمتبادل.",explanation:"إتقان التكرار مهارة تميّز المحترفين. التكرار الذيلي (tail recursion) يضع الاستدعاء الذاتي آخر شيء، مما يتيح للمترجم تحويله لحلقة وتجنب Stack Overflow. أبراج هانوي مثال كلاسيكي يوضح قوة التكرار في حل مسائل معقدة بكود بسيط. توليد التباديل والتوافيق يظهر في مسائل مثل الجدولة والتشفير.",code:`# التكرار الذيلي — نتيجة كمعامل
دالة مضروب(ن، مركم)
    إذا (ن <= 1)
        ارجع مركم
    نهاية
    ارجع مضروب(ن - 1، ن * مركم)
نهاية

اطبع(مضروب(5، 1))  # 120

# أبراج هانوي
دالة هانوي(ن، من، إلى، مساعد)
    إذا (ن == 1)
        اطبع("انقل " + من + " → " + إلى)
        ارجع لاشيء
    نهاية
    هانوي(ن - 1، من، مساعد، إلى)
    اطبع("انقل " + من + " → " + إلى)
    هانوي(ن - 1، مساعد، إلى، من)
نهاية

هانوي(3، "أ"، "ج"، "ب")`,exercise:`# ولّد كل التباديل لمصفوفة [1، 2، 3]
دالة تباديل(مصفوفة)
    إذا (طول(مصفوفة) <= 1)
        ارجع [مصفوفة]
    نهاية
    متغير نتائج = []
    # لكل عنصر: ثبّته وولّد تباديل الباقي
    ارجع نتائج
نهاية

اطبع(طول(تباديل([1، 2، 3])))  # 6`,hint:"لكل عنصر، أزله من المصفوفة وولّد تباديل الباقي وألحقه في البداية.",tips:["التكرار الذيلي يمرر النتيجة كمعامل","أبراج هانوي مثال كلاسيكي","تأكد من الاقتراب من حالة الأساس"],concepts:["تكرار ذيلي","Tail Recursion","أبراج هانوي"]},{id:"l5_26",title:"البرمجة الديناميكية",desc:"تقسيم المسائل لمسائل فرعية متداخلة وتخزين حلولها.",explanation:"البرمجة الديناميكية من أقوى تقنيات حل المسائل — تحوّل المستحيل لممكن. الفكرة: إذا حللت مسألة فرعية مرة، لا تحلها ثانية — خزّنها. فيبوناتشي البطيء O(2^n) يصبح O(n). تُستخدم في: أقصر مسار، حزمة الظهر (Knapsack)، تعديل النصوص (Edit Distance). من أهم مواضيع مقابلات FAANG.",code:`# فيبوناتشي — من الأسفل للأعلى
دالة فيبو(ن)
    إذا (ن <= 1)
        ارجع ن
    نهاية
    متغير جدول = [0، 1]
    لكل ي في نطاق(2، ن + 1)
        جدول += [جدول[ي - 1] + جدول[ي - 2]]
    نهاية
    ارجع جدول[ن]
نهاية

اطبع(فيبو(10))  # 55

# صعود الدرج — كم طريقة لـ ن درجة؟
دالة طرق_درج(ن)
    إذا (ن <= 2)
        ارجع ن
    نهاية
    متغير ج = [0، 1، 2]
    لكل ي في نطاق(3، ن + 1)
        ج += [ج[ي - 1] + ج[ي - 2]]
    نهاية
    ارجع ج[ن]
نهاية

اطبع(طرق_درج(5))  # 8`,exercise:`# حل أقصى مجموع فرعي (Kadane)
دالة أقصى_مجموع(مصفوفة)
    متغير حالي = مصفوفة[0]
    متغير كلي = مصفوفة[0]
    # أكمل: لكل عنصر من الثاني
    # حالي = أكبر(عنصر، حالي + عنصر)
    # كلي = أكبر(كلي، حالي)
    ارجع كلي
نهاية

اطبع(أقصى_مجموع([-2، 1، -3، 4، -1، 2، 1]))`,hint:"لكل عنصر: حالي = أكبر(العنصر، حالي + العنصر) ثم حدّث كلي.",tips:["تحل المسائل المتداخلة فرعياً","من الأسفل للأعلى تبني الحلول تدريجياً","التخزين يمنع إعادة الحساب"],concepts:["برمجة ديناميكية","Dynamic Programming","Kadane"]},{id:"l5_27",title:"الخوارزميات الجشعة (Greedy)",desc:"اتخاذ الخيار الأفضل محلياً في كل خطوة.",explanation:"الخوارزميات الجشعة تختار الأفضل الآن بدون التفكير في المستقبل. ليست مثالية دائماً، لكنها سريعة وبسيطة. مثال الفكة: اختر أكبر عملة ممكنة دائماً. جدولة الأنشطة: اختر الذي ينتهي أولاً. تعمل عندما يكون الاختيار المحلي لا يؤثر على الخيارات المستقبلية (greedy choice property). Huffman coding مثال شهير.",code:`# الفكّة — أقل عدد من العملات
دالة أقل_فكّة(مبلغ، عملات)
    متغير نتيجة = []
    متغير متبقي = مبلغ
    لكل عملة في عملات
        بينما (متبقي >= عملة)
            نتيجة += [عملة]
            متبقي -= عملة
        نهاية
    نهاية
    ارجع نتيجة
نهاية

متغير عملات = [100، 50، 20، 10، 5، 1]
اطبع(أقل_فكّة(87، عملات))
# [50، 20، 10، 5، 1، 1]`,exercise:`# جدولة أنشطة غير متداخلة
# كل نشاط: [بداية، نهاية]، مرتبة بالنهاية
دالة جدولة(أنشطة)
    متغير مختارة = [أنشطة[0]]
    متغير آخر = أنشطة[0][1]
    # أكمل: أضف النشاط إذا بدايته >= آخر
    ارجع مختارة
نهاية

متغير أ = [[1،3]، [2،5]، [4،7]، [6،9]]
اطبع(طول(جدولة(أ)))`,hint:"لكل نشاط: إذا بدايته >= نهاية آخر مختار، أضفه.",tips:["تختار الأفضل محلياً كل مرة","لا تضمن الأمثل دائماً لكنها سريعة","مثالية للفكّة والجدولة"],concepts:["خوارزمية جشعة","Greedy","اختيار محلي"]},{id:"l5_28",title:"تقنية المؤشرَين (Two Pointers)",desc:"مؤشران يتحركان في المصفوفة لحل مسائل بكفاءة.",explanation:"تقنية المؤشرين تحوّل O(n²) إلى O(n) لكثير من المسائل. الفكرة: مؤشران يتحركان بدلاً من حلقتين متداخلتين. البحث عن زوج مجموعهما X: مؤشر من البداية وآخر من النهاية. حذف التكرارات: مؤشران يتبعان بعضهما. من أكثر التقنيات طلباً في مقابلات LeetCode.",code:`# البحث عن زوج مجموعهما = هدف
دالة زوج_المجموع(مرتبة، هدف)
    متغير يسار = 0
    متغير يمين = طول(مرتبة) - 1
    بينما (يسار < يمين)
        متغير مج = مرتبة[يسار] + مرتبة[يمين]
        إذا (مج == هدف)
            ارجع [يسار، يمين]
        وإلا
            إذا (مج < هدف)
                يسار += 1
            وإلا
                يمين -= 1
            نهاية
        نهاية
    نهاية
    ارجع لاشيء
نهاية

متغير أ = [1، 3، 5، 7، 9، 11]
اطبع(زوج_المجموع(أ، 12))  # [1، 4]`,exercise:`# حذف التكرارات من مصفوفة مرتبة
دالة بدون_تكرار(مصفوفة)
    إذا (طول(مصفوفة) == 0)
        ارجع []
    نهاية
    متغير نتيجة = [مصفوفة[0]]
    # أكمل: أضف فقط إذا اختلف عن الأخير
    ارجع نتيجة
نهاية

اطبع(بدون_تكرار([1،1،2،2،3،4،4]))`,hint:"كرر من العنصر الثاني. إذا اختلف عن آخر عنصر بالنتيجة أضفه.",tips:["يحول O(n²) إلى O(n)","يعمل على مصفوفات مرتبة عادة","مؤشر من كل طرف أو اثنان من البداية"],concepts:["مؤشران","Two Pointers","O(n)"]},{id:"l5_29",title:"النافذة المنزلقة (Sliding Window)",desc:"معالجة نوافذ فرعية متتابعة من مصفوفة بكفاءة.",explanation:"النافذة المنزلقة مثل نافذة تتحرك على البيانات: بدلاً من إعادة حساب مجموع كل نافذة من الصفر، أضف عنصر يمين وأزل عنصر يسار. هذا يحوّل O(n*k) إلى O(n). تُستخدم في: أقصى مجموع لنافذة، أصغر نافذة تحقق شرطاً، عد الحروف الفريدة. نوعان: ثابتة الحجم ومرنة.",code:`# أقصى مجموع لنافذة بحجم ك
دالة أقصى_نافذة(مصفوفة، ك)
    متغير مجموع = 0
    لكل ي في نطاق(0، ك)
        مجموع += مصفوفة[ي]
    نهاية
    متغير أقصى = مجموع
    لكل ي في نطاق(ك، طول(مصفوفة))
        مجموع += مصفوفة[ي] - مصفوفة[ي - ك]
        إذا (مجموع > أقصى)
            أقصى = مجموع
        نهاية
    نهاية
    ارجع أقصى
نهاية

متغير ب = [1، 4، 2، 10، 2، 3، 1، 0، 20]
اطبع(أقصى_نافذة(ب، 3))  # 23`,exercise:`# أوجد أصغر نافذة مجموعها >= هدف
دالة أصغر_نافذة(مصفوفة، هدف)
    متغير أصغر = طول(مصفوفة) + 1
    متغير مج = 0
    متغير بداية = 0
    # أكمل: وسّع يميناً وقلّص يساراً
    # حدّث أصغر عند مج >= هدف
    ارجع أصغر
نهاية

اطبع(أصغر_نافذة([2، 3، 1، 2، 4، 3]، 7))`,hint:"وسّع بإضافة عناصر حتى المجموع >= هدف. ثم قلّص من اليسار.",tips:["تتجنب إعادة حساب المجموع كل مرة","ثابتة: أضف يمين وأزل يسار","مرنة: وسّع وقلّص حسب الشرط"],concepts:["نافذة منزلقة","Sliding Window","O(n)"]},{id:"l5_30",title:"تعقيد الخوارزميات والتحسين",desc:"تحليل التعقيد الزمني واختيار الخوارزمية الأفضل.",explanation:"فهم Big O ضروري لكل مبرمج محترف. O(1) ثابت (hash lookup)، O(log n) لوغاريتمي (بحث ثنائي)، O(n) خطي (بحث تسلسلي)، O(n²) تربيعي (حلقتان متداخلتان). الهدف: تحويل التربيعي لخطي أو لوغاريتمي باستخدام هياكل بيانات مناسبة أو تقنيات مثل المؤشرين. التعقيد المكاني مهم أيضاً.",code:`# مقارنة تعقيدات
# O(n) — خطي
دالة بحث_خطي(مصفوفة، هدف)
    لكل ي في نطاق(0، طول(مصفوفة))
        إذا (مصفوفة[ي] == هدف)
            ارجع ي
        نهاية
    نهاية
    ارجع -1
نهاية

# O(n²) — تربيعي
دالة أزواج(مصفوفة)
    متغير عدد = 0
    لكل ي في نطاق(0، طول(مصفوفة))
        لكل ج في نطاق(ي + 1، طول(مصفوفة))
            عدد += 1
        نهاية
    نهاية
    ارجع عدد
نهاية

اطبع(بحث_خطي([5، 3، 8، 1]، 8))  # 2
اطبع(أزواج([1، 2، 3، 4]))  # 6`,exercise:`# حسّن البحث عن زوج من O(n²) إلى O(n)
# الأصلي: حلقتان متداخلتان
# المحسّن: خريطة

دالة ابحث_زوج(مصفوفة، هدف)
    متغير مرئي = {}
    # لكل عنصر: المكمّل = هدف - عنصر
    # إذا المكمّل بالخريطة → أرجع الزوج
    # وإلا خزّن العنصر
نهاية

اطبع(ابحث_زوج([2، 7، 11، 15]، 9))`,hint:"لكل عنصر احسب المكمّل. إذا بالخريطة أرجعه وإلا خزّن العنصر.",tips:["O(1) < O(log n) < O(n) < O(n²)","خريطة تحول O(n²) إلى O(n)","التعقيد المكاني مهم أيضاً"],concepts:["تعقيد زمني","Big O","تحسين"]}]},Ic={id:"l5_libraries_modules",title:"بناء المكتبات والوحدات",icon:"📦",lessons:[{id:"l5_31",title:"مبادئ تصميم الوحدات",desc:"تنظيم الكود في وحدات مستقلة ذات مسؤولية واحدة.",explanation:"مبدأ المسؤولية الواحدة (Single Responsibility) أساس الهندسة الجيدة: كل وحدة تفعل شيئاً واحداً وتفعله جيداً. مثل أدوات المطبخ: السكين للقطع، الملعقة للتحريك. الفوائد: سهولة الاختبار، إعادة الاستخدام، وقلة الارتباط (coupling). صدّر الواجهة العامة فقط وأخفِ التفاصيل. هذا المبدأ أول SOLID وأهمها.",code:`# وحدة رياضيات — مسؤولية واحدة
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

دالة قسمة(أ، ب)
    إذا (ب == 0)
        ارمي "القسمة على صفر"
    نهاية
    ارجع أ / ب
نهاية

صدّر جمع
صدّر قسمة`,exercise:`# صمم وحدة تحويل وحدات
دالة سلسيوس_فهرنهايت(د)
    ارجع د * 9 / 5 + 32
نهاية

# أكمل: فهرنهايت → سلسيوس
# أكمل: كيلومتر ↔ ميل
صدّر سلسيوس_فهرنهايت`,hint:"أنشئ دالة لكل تحويل واستخدم صدّر للدوال العامة.",tips:["كل وحدة مسؤولية واحدة واضحة","صدّر الواجهة العامة المطلوبة فقط","أخفِ التفاصيل الداخلية"],concepts:["تصميم وحدات","مسؤولية واحدة","تصدير"]},{id:"l5_32",title:"تصميم واجهة برمجية (API)",desc:"بناء واجهات واضحة وسهلة الاستخدام ومتسقة.",explanation:'API الجيدة مثل المطعم الراقي: قائمة واضحة، أسماء مفهومة، تجربة متسقة. نمط CRUD (إنشاء/قراءة/تحديث/حذف) معيار الصناعة. الأخطاء يجب أن تكون واضحة وقابلة للمعالجة. قاعدة ذهبية: المستخدم لا يحتاج قراءة الكود لفهم كيفية الاستخدام. كتب مثل "API Design Patterns" تشرح التفاصيل.',code:`# واجهة CRUD متسقة
صنف مخزن
    باني()
        هذا.بيانات = {}
    نهاية
    دالة أنشئ(مفتاح، قيمة)
        هذا.بيانات[مفتاح] = قيمة
    نهاية
    دالة اقرأ(مفتاح)
        ارجع هذا.بيانات[مفتاح]
    نهاية
    دالة احذف(مفتاح)
        هذا.بيانات[مفتاح] = لاشيء
    نهاية
نهاية

متغير م = جديد مخزن()
م.أنشئ("اسم"، "أحمد")
اطبع(م.اقرأ("اسم"))`,exercise:`# صمم واجهة لقائمة مهام
صنف قائمة_مهام
    باني()
        هذا.مهام = {}
    نهاية
    دالة أضف(عنوان)
        # أنشئ مهمة
    نهاية
    دالة أكمل(رقم)
        # علّم كمكتملة
    نهاية
نهاية`,hint:"خزّن كل مهمة كخريطة بعنوان وحالة.",tips:["الواجهة الجيدة بسيطة ومتسقة","اتبع نمط CRUD","ارمِ أخطاء واضحة"],concepts:["تصميم API","واجهة برمجية","CRUD"]},{id:"l5_33",title:"أصناف قابلة لإعادة الاستخدام",desc:"بناء أصناف مرنة تدعم سلسلة الاستدعاءات.",explanation:"سلسلة الاستدعاءات (Method Chaining) تجعل الكود يُقرأ كجملة: user.filter(active).sort(byDate).limit(10). السر: كل دالة تُرجع الكائن نفسه أو نسخة جديدة. مكتبات مثل jQuery وLodash وRxJS مبنية على هذا النمط. الفائدة: كود أقل، قراءة أفضل، وتجنب المتغيرات الوسيطة غير الضرورية.",code:`# قائمة ذكية بسلسلة استدعاءات
صنف قائمة_ذكية
    باني(عناصر)
        هذا.عناصر = عناصر
    نهاية
    دالة رشّح(شرط)
        متغير ن = []
        لكل ع في هذا.عناصر
            إذا (شرط(ع))
                ن += [ع]
            نهاية
        نهاية
        ارجع جديد قائمة_ذكية(ن)
    نهاية
    دالة عيّن(تحويل)
        متغير ن = []
        لكل ع في هذا.عناصر
            ن += [تحويل(ع)]
        نهاية
        ارجع جديد قائمة_ذكية(ن)
    نهاية
نهاية

متغير أ = جديد قائمة_ذكية([1، 2، 3، 4، 5])
متغير ن = أ.رشّح(لامدا(ن) => ن > 2)
              .عيّن(لامدا(ن) => ن * 10)
اطبع(ن.عناصر)  # [30، 40، 50]`,exercise:`# أنشئ صنف منسّق نص بسلسلة
صنف منسّق
    باني(نص)
        هذا.نص = نص
    نهاية
    دالة بأقواس()
        ارجع جديد منسّق("(" + هذا.نص + ")")
    نهاية
    دالة بنجوم()
        # أحط النص بنجوم
    نهاية
    دالة قيمة()
        ارجع هذا.نص
    نهاية
نهاية

اطبع(جديد منسّق("مرحبا").بأقواس().بنجوم().قيمة())`,hint:"كل دالة تُرجع كائن منسّق جديد لتمكين السلسلة.",tips:["إرجاع نسخة جديدة يمكّن سلسلة الاستدعاءات","اجعل الواجهة مستقلة عن حالة الاستخدام","الأصناف المرنة توفر الوقت والجهد"],concepts:["إعادة الاستخدام","Method Chaining","سلسلة"]},{id:"l5_34",title:"إدارة الإعدادات (Configuration)",desc:"نظام إعدادات مرن يدعم القيم الافتراضية والتخصيص.",explanation:"إدارة الإعدادات مهارة مهملة لكنها حاسمة. المبدأ: افصل الإعدادات عن الكود. القيم الافتراضية ضرورية للعمل بدون إعداد. البيئات (dev/staging/prod) ترث من بعضها وتخصص الفروقات. الأسرار (كلمات المرور، المفاتيح) لا تُخزّن في الكود أبداً — استخدم متغيرات البيئة.",code:`# نظام إعدادات بقيم افتراضية
صنف إعدادات
    باني()
        هذا.قيم = {}
        هذا.افتراضية = {}
    نهاية
    دالة عيّن_افتراضي(م، ق)
        هذا.افتراضية[م] = ق
    نهاية
    دالة عيّن(م، ق)
        هذا.قيم[م] = ق
    نهاية
    دالة اقرأ(م)
        إذا (هذا.قيم[م] != لاشيء)
            ارجع هذا.قيم[م]
        نهاية
        ارجع هذا.افتراضية[م]
    نهاية
نهاية

متغير إ = جديد إعدادات()
إ.عيّن_افتراضي("لغة"، "العربية")
إ.عيّن("خط"، 18)
اطبع(إ.اقرأ("لغة"))  # العربية`,exercise:`# إعدادات متعددة البيئات مع وراثة
صنف إعدادات_بيئة
    باني(اسم، أساس)
        هذا.اسم = اسم
        هذا.أساس = أساس
        هذا.قيم = {}
    نهاية
    دالة عيّن(مفتاح، قيمة)
        هذا.قيم[مفتاح] = قيمة
    نهاية
    دالة اقرأ(مفتاح)
        # أكمل: ابحث محلياً ثم في الأساس
    نهاية
نهاية`,hint:"إذا لم يوجد محلياً والأساس != لاشيء، ابحث في الأساس.",tips:["افصل الافتراضية عن المخصصة","دعم البيئات بالوراثة","المحلي يُلغي الافتراضي"],concepts:["إدارة إعدادات","Configuration","قيم افتراضية"]},{id:"l5_35",title:"مكتبة التسجيل (Logging)",desc:"نظام تسجيل مرن بمستويات مختلفة.",explanation:"التسجيل عيون المبرمج في الإنتاج: بدونه لا تعرف ماذا يحدث. المستويات القياسية: DEBUG للتطوير، INFO للأحداث العادية، WARN للمخاطر، ERROR للأخطاء. الحد الأدنى يتحكم بما يُعرض. في الإنتاج، السجلات تُرسل لأنظمة مثل ELK أو Datadog للتحليل والتنبيهات.",code:`# مكتبة تسجيل بمستويات
صنف مسجّل
    باني(اسم، حد)
        هذا.اسم = اسم
        هذا.حد = حد
        هذا.سجلات = []
    نهاية
    دالة _سجّل(مستوى، رسالة)
        إذا (مستوى >= هذا.حد)
            متغير سطر = "[" + هذا.اسم + "] " + رسالة
            هذا.سجلات += [سطر]
            اطبع(سطر)
        نهاية
    نهاية
    دالة معلومات(ر)
        هذا._سجّل(1، "معلومات: " + ر)
    نهاية
    دالة خطأ(ر)
        هذا._سجّل(3، "خطأ: " + ر)
    نهاية
نهاية

متغير س = جديد مسجّل("تطبيق"، 1)
س.معلومات("بدء")
س.خطأ("فشل")`,exercise:`# أضف إحصائيات وتصفية للمسجّل
صنف مسجّل_متقدم يرث مسجّل
    باني(اسم)
        الأساس(اسم، 0)
        هذا.عدّاد = {"معلومات": 0، "خطأ": 0}
    نهاية
    # دالة إحصائيات — اعرض الأعداد
    # دالة رشّح(مستوى) — أرجع سجلات مستوى معين
نهاية

متغير م = جديد مسجّل_متقدم("نظام")`,hint:"زد العدّاد عند كل تسجيل. رشّح بالبحث في مصفوفة السجلات.",tips:["المستويات تصفي حسب الأهمية","السجلات المخزنة تفيد للتحليل","الحد الأدنى يتحكم بما يُعرض"],concepts:["تسجيل","Logging","مستويات"]},{id:"l5_36",title:"مكتبة التحقق (Validation)",desc:"نظام تحقق من صحة البيانات بقواعد قابلة للتركيب.",explanation:'التحقق من البيانات أول خط دفاعي: "لا تثق بمدخلات المستخدم أبداً". القواعد القابلة للتركيب أفضل: required().minLength(3).email(). اجمع كل الأخطاء بدل التوقف عند أول خطأ. رسائل واضحة تساعد المستخدم على التصحيح. مكتبات مثل Joi وZod في JavaScript تطبِق هذا النمط.',code:`# محقق بقواعد مسلسلة
صنف محقق
    باني()
        هذا.أخطاء = []
    نهاية
    دالة مطلوب(اسم، قيمة)
        إذا (قيمة == لاشيء أو قيمة == "")
            هذا.أخطاء += [اسم + " مطلوب"]
        نهاية
        ارجع هذا
    نهاية
    دالة حد_أدنى(اسم، قيمة، حد)
        إذا (قيمة < حد)
            هذا.أخطاء += [اسم + " >= " + إلى_نص(حد)]
        نهاية
        ارجع هذا
    نهاية
    دالة صالح()
        ارجع طول(هذا.أخطاء) == 0
    نهاية
نهاية

متغير م = جديد محقق()
م.مطلوب("الاسم"، "أحمد")
م.حد_أدنى("العمر"، 15، 18)
اطبع(م.صالح())  # خطأ
اطبع(م.أخطاء)`,exercise:`# تحقق من نموذج تسجيل مستخدم
دالة تحقق(اسم، عمر، بريد)
    متغير أخطاء = []
    # اسم: مطلوب، 3+ حروف
    # عمر: 13-120
    # بريد: يحتوي @
    ارجع أخطاء
نهاية

اطبع(تحقق("أ"، 10، "بدون"))`,hint:"تحقق من كل حقل بشروطه وأضف رسالة خطأ لكل انتهاك.",tips:["إرجاع هذا يتيح سلسلة القواعد","اجمع كل الأخطاء لا فقط الأولى","رسائل واضحة تساعد المستخدم"],concepts:["تحقق","Validation","قواعد مركّبة"]},{id:"l5_37",title:"تسلسل البيانات (Serialization)",desc:"تحويل الكائنات إلى نصوص والعكس للحفظ أو النقل.",explanation:"التسلسل يحوّل الكائنات الحية لنص يمكن حفظه أو إرساله. JSON الصيغة الأكثر شيوعاً للـ APIs. التحديات: المراجع الدائرية، الدوال، الأنواع المخصصة. الصيغ الثنائية (Protocol Buffers، MessagePack) أكفأ للأنظمة الكبيرة. التوافق العكسي (backward compatibility) مهم عند تطوير الصيغة.",code:`# تسلسل — تحويل لنص
دالة سلسل(كائن)
    إذا (نوع(كائن) == "رقم")
        ارجع إلى_نص(كائن)
    وإلا
        إذا (نوع(كائن) == "نص")
            ارجع """ + كائن + """
        وإلا
            إذا (نوع(كائن) == "مصفوفة")
                متغير أ = []
                لكل ع في كائن
                    أ += [سلسل(ع)]
                نهاية
                ارجع "[" + أ + "]"
            نهاية
        نهاية
    نهاية
    ارجع "لاشيء"
نهاية

اطبع(سلسل(42))
اطبع(سلسل("مرحبا"))
اطبع(سلسل([1، 2، 3]))`,exercise:`# سلسل خريطة لصيغة مفتاح=قيمة
دالة سلسل_خريطة(خ)
    متغير سطور = []
    # لكل مفتاح: أضف "مفتاح=قيمة"
    ارجع سطور
نهاية

دالة فكّ(سطور)
    # حوّل مصفوفة "مفتاح=قيمة" لخريطة
نهاية

متغير ب = {"اسم": "أحمد"، "عمر": "25"}
اطبع(سلسل_خريطة(ب))`,hint:'لكل مفتاح أنشئ "مفتاح=قيمة". للفك قسّم على "=".',tips:["التسلسل يحوّل لصيغة قابلة للنقل","فك التسلسل يعيد البناء من النص","اختر صيغة واضحة وسهلة التحليل"],concepts:["تسلسل","Serialization","تحويل بيانات"]},{id:"l5_38",title:"نظام الأحداث (Event System)",desc:"نظام اشتراك وإشعار بدون ارتباط مباشر بين المكونات.",explanation:"نمط المراقب (Observer/Pub-Sub) يفصل المُرسل عن المستقبل. الفائدة: إضافة مستمعين بدون تعديل المُرسل. كل عنصر HTML يستخدمه: addEventListener. في الخوادم، أنظمة مثل Redis Pub/Sub وKafka مبنية على هذا. مهم لبناء أنظمة مرنة وقابلة للتوسع (loosely coupled).",code:`# نظام أحداث — نمط المراقب
صنف ناشر
    باني()
        هذا.مستمعون = {}
    نهاية
    دالة اشترك(حدث، معالج)
        إذا (هذا.مستمعون[حدث] == لاشيء)
            هذا.مستمعون[حدث] = []
        نهاية
        هذا.مستمعون[حدث] += [معالج]
    نهاية
    دالة أطلق(حدث، بيانات)
        إذا (هذا.مستمعون[حدث] != لاشيء)
            لكل م في هذا.مستمعون[حدث]
                م(بيانات)
            نهاية
        نهاية
    نهاية
نهاية

متغير ن = جديد ناشر()
ن.اشترك("تسجيل"، لامدا(اسم) => اطبع("مرحباً " + اسم))
ن.اشترك("تسجيل"، لامدا(اسم) => اطبع("بريد لـ " + اسم))
ن.أطلق("تسجيل"، "سارة")`,exercise:`# أضف "مرة واحدة" وعدد المستمعين
صنف أحداث يرث ناشر
    باني()
        الأساس()
    نهاية
    دالة مرة_واحدة(حدث، معالج)
        # اشترك وأزل بعد أول تنفيذ
    نهاية
    دالة عدد(حدث)
        # أرجع عدد المستمعين
    نهاية
نهاية

متغير أ = جديد أحداث()
أ.مرة_واحدة("نقر"، لامدا(ب) => اطبع("نُقر"))`,hint:"غلّف المعالج بلامدا تستدعيه ثم تزيل نفسها.",tips:["يفصل المُرسل عن المستقبل","إضافة مستمعين بدون تغيير الكود","أساس نمط المراقب (Observer)"],concepts:["نظام أحداث","Events","Observer"]},{id:"l5_39",title:"هندسة الإضافات (Plugins)",desc:"تصميم نظام يقبل إضافات لتوسيع وظائفه.",explanation:"هندسة الإضافات تجعل النظام قابلاً للتوسع بدون تعديل النواة. VS Code مثال مثالي: المحرر بسيط والإضافات تضيف كل شيء. المفتاح: واجهة واضحة للإضافات (Hooks/Lifecycle) ونقاط توسع محددة. الأمان: عزل الإضافات لمنع تأثيرها السلبي على النواة.",code:`# نظام إضافات
صنف نظام
    باني()
        هذا.إضافات = []
    نهاية
    دالة سجّل(إضافة)
        هذا.إضافات += [إضافة]
        إذا (إضافة["تهيئة"] != لاشيء)
            إضافة["تهيئة"]()
        نهاية
    نهاية
    دالة نفّذ(أمر، بيانات)
        متغير نتيجة = بيانات
        لكل إ في هذا.إضافات
            إذا (إ["عالج"] != لاشيء)
                نتيجة = إ["عالج"](أمر، نتيجة)
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير ن = جديد نظام()
ن.سجّل({
    "تهيئة": لامدا() => اطبع("تم التحميل"),
    "عالج": لامدا(أ، ب) => ب
})
ن.نفّذ("حفظ"، "بيانات")`,exercise:`# أنشئ معالج نصوص بإضافات (مرشّحات)
صنف معالج
    باني()
        هذا.مرشّحات = []
    نهاية
    دالة أضف(مرشّح)
        هذا.مرشّحات += [مرشّح]
    نهاية
    دالة عالج(نص)
        # مرر النص عبر كل المرشّحات
    نهاية
نهاية

# أضف مرشّحات واختبر`,hint:"كرر على المرشّحات وطبق كل واحدة على النتيجة بالتتابع.",tips:["الإضافات توسّع بدون تعديل النواة","واجهة موحدة تسهّل التوسيع","سلسلة المعالجة كنمط الأنبوب"],concepts:["إضافات","Plugin Architecture","توسيع"]},{id:"l5_40",title:"تنظيم ونشر الوحدات",desc:"أفضل الممارسات لتنظيم وتوثيق المكتبات.",explanation:"المكتبة الجيدة ليست فقط كوداً يعمل — بل موثقة ومختبرة وسهلة التثبيت. التوثيق: README واضحة، أمثلة عملية، API reference. الإصدارات: Semantic Versioning (MAJOR.MINOR.PATCH) لتوضيح التغييرات. الترخيص: اختر رخصة مناسبة (MIT، Apache). الاختبارات: تغطية عالية تعطي ثقة.",code:`# مكتبة رياضيات منظمة
دالة قوة(أساس، أس)
    متغير نتيجة = 1
    لكل ي في نطاق(0، أس)
        نتيجة *= أساس
    نهاية
    ارجع نتيجة
نهاية

دالة متوسط(أرقام)
    متغير مج = 0
    لكل ن في أرقام
        مج += ن
    نهاية
    ارجع مج / طول(أرقام)
نهاية

دالة أكبر_في(أرقام)
    متغير أ = أرقام[0]
    لكل ن في أرقام
        إذا (ن > أ)
            أ = ن
        نهاية
    نهاية
    ارجع أ
نهاية

صدّر قوة
صدّر متوسط
صدّر أكبر_في`,exercise:`# نظّم مكتبة نصوص كاملة
دالة عد_كلمات(نص)
    # أكمل: قسّم على المسافات وعُد
نهاية

دالة عكس_نص(نص)
    # أكمل: كرر من النهاية
نهاية

دالة يحتوي(نص، فرعي)
    # أكمل: ابحث عن النص الفرعي
نهاية

صدّر عد_كلمات
صدّر عكس_نص`,hint:"للعد قسّم على المسافات. للعكس كرر من النهاية.",tips:["نظّم حسب الوظيفة (رياضيات، نصوص)","وثّق كل دالة عامة","صدّر الواجهة العامة فقط"],concepts:["تنظيم مكتبات","نشر وحدات","توثيق"]}]},wc={id:"l5_cli_applications",title:"تطبيقات سطر الأوامر",icon:"💻",lessons:[{id:"l5_41",title:"محلل الأوامر",desc:"بناء محلل يحوّل نص الأمر إلى أجزاء مفهومة للبرنامج.",explanation:'محلل الأوامر أساس كل أداة CLI احترافية. مثل git: "git commit -m message" يُحلل لـ {command: commit, flags: {m: message}}. المعيار: POSIX للخيارات القصيرة (-v) والطويلة (--verbose). أنماط شائعة: commands/subcommands (docker build)، وسائط موقعية (file.txt)، وخيارات بقيم (--output=file).',code:`# محلل أوامر بسيط
صنف محلل_أوامر
    باني()
        هذا.أوامر = {}
    نهاية
    دالة سجّل(اسم، وصف)
        هذا.أوامر[اسم] = وصف
    نهاية
    دالة حلل(نص)
        متغير أجزاء = نص
        متغير أمر = أجزاء
        ارجع {"أمر": أمر، "نص": نص}
    نهاية
نهاية

متغير م = جديد محلل_أوامر()
م.سجّل("مساعدة"، "عرض المساعدة")
اطبع(م.حلل("مساعدة"))`,exercise:`# أنشئ محلل مع وسائط
صنف محلل
    باني()
        هذا.أوامر = {}
    نهاية
    دالة سجّل(اسم، دالة_معالجة)
        هذا.أوامر[اسم] = دالة_معالجة
    نهاية
    # أكمل: دالة نفّذ(نص) تستدعي المعالج
نهاية`,hint:"استخدم هذا.أوامر[اسم] للوصول للمعالج ثم استدعِه.",tips:["فصل التحليل عن التنفيذ","خزّن الأوامر في خريطة","تحقق من وجود الأمر قبل التنفيذ"],concepts:["تحليل أوامر","خرائط","نمط الأمر"]},{id:"l5_42",title:"معالجة الوسائط",desc:"استخراج ومعالجة الوسائط والخيارات من مدخلات المستخدم.",explanation:'معالجة الوسائط تحوّل النص لقيم مطبوعة ومتحقق منها. القيم الافتراضية ضرورية: المستخدم لا يوفر كل شيء. التحقق من الأنواع: "--count 5" يجب أن يكون رقماً. الخيارات المطلوبة مقابل الاختيارية. مكتبات مثل Commander.js وyargs توفر هذا جاهزاً.',code:`# معالج وسائط مع قيم افتراضية
صنف معالج_وسائط
    باني()
        هذا.خيارات = {}
        هذا.افتراضي = {}
    نهاية
    دالة أضف_خيار(اسم، قيمة_افتراضية)
        هذا.افتراضي[اسم] = قيمة_افتراضية
    نهاية
    دالة احصل(اسم)
        إذا (هذا.خيارات[اسم] != لاشيء)
            ارجع هذا.خيارات[اسم]
        نهاية
        ارجع هذا.افتراضي[اسم]
    نهاية
نهاية

متغير م = جديد معالج_وسائط()
م.أضف_خيار("لون"، "أزرق")
اطبع(م.احصل("لون"))`,exercise:`# أنشئ معالج مع تحقق من النوع
صنف خيارات
    باني()
        هذا.قيم = {}
        هذا.أنواع = {}
    نهاية
    دالة عرّف(اسم، نوع، افتراضي)
        هذا.أنواع[اسم] = نوع
        هذا.قيم[اسم] = افتراضي
    نهاية
    # أكمل: دالة عيّن(اسم، قيمة) مع تحقق
نهاية`,hint:"تحقق من نوع(قيمة) ضد هذا.أنواع[اسم] قبل التعيين.",tips:["دعم القيم الافتراضية ضروري","تحقق من أنواع الوسائط مبكراً","وفّر رسائل خطأ واضحة"],concepts:["معالجة وسائط","قيم افتراضية","تحقق أنواع"]},{id:"l5_43",title:"القوائم التفاعلية",desc:"بناء قائمة تفاعلية تعرض خيارات وتستجيب لاختيار المستخدم.",explanation:'القوائم التفاعلية تجعل CLI سهل الاستخدام للمبتدئين. الترقيم يسهّل الاختيار. القوائم الفرعية مع خيار "0. رجوع" أساسية. تحقق من صحة الاختيار وأظهر رسالة عند الخطأ. مكتبات مثل inquirer.js توفر قوائم متقدمة بالأسهم والبحث.',code:`# نظام قائمة تفاعلية
صنف قائمة
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.خيارات = []
    نهاية
    دالة أضف(نص، معالج)
        هذا.خيارات += [{"نص": نص، "معالج": معالج}]
    نهاية
    دالة اعرض()
        اطبع_سطر("=== " + هذا.عنوان + " ===")
        لكل ت في نطاق(طول(هذا.خيارات))
            اطبع_سطر(إلى_نص(ت + 1) + ". " + هذا.خيارات[ت]["نص"])
        نهاية
    نهاية
نهاية

متغير ق = جديد قائمة("القائمة الرئيسية")
ق.أضف("عرض البيانات"، لامدا() اطبع("عرض") نهاية)
ق.أضف("خروج"، لامدا() اطبع("وداعاً") نهاية)
ق.اعرض()`,exercise:`# أنشئ قائمة متعددة المستويات
صنف قائمة_فرعية
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.عناصر = []
    نهاية
    دالة أضف(نص)
        هذا.عناصر += [نص]
    نهاية
    # أكمل: دالة اعرض() بترقيم
نهاية`,hint:"استخدم لكل مع نطاق(طول(...)) للترقيم التسلسلي.",tips:["رقّم الخيارات ليسهل الاختيار","أضف خيار رجوع في القوائم الفرعية","تحقق من صحة الاختيار"],concepts:["قوائم تفاعلية","واجهة مستخدم نصية","لامدا"]},{id:"l5_44",title:"معالج الملفات النصية",desc:"بناء أداة لمعالجة وتحويل الملفات النصية.",explanation:"معالجة النصوص مهارة أساسية لـ CLI. أدوات Unix الكلاسيكية (grep, sed, awk) مبنية على هذا. المبدأ: اقرأ سطراً بسطر لتجنب استهلاك الذاكرة. الإحصائيات (عد سطور، كلمات، أحرف) مفيدة. التصفية والتحويل يجب أن تعمل على نسخة للحفاظ على الأصل.",code:`# معالج نصوص بأوامر
صنف معالج_نصوص
    باني(محتوى)
        هذا.سطور = محتوى
    نهاية
    دالة عدد_سطور()
        ارجع طول(هذا.سطور)
    نهاية
    دالة ابحث(كلمة)
        متغير نتائج = []
        لكل ت في نطاق(طول(هذا.سطور))
            نتائج += [ت]
        نهاية
        ارجع نتائج
    نهاية
    دالة إحصائيات()
        ارجع {"سطور": هذا.عدد_سطور()}
    نهاية
نهاية

متغير م = جديد معالج_نصوص(["سطر1"، "سطر2"، "سطر3"])
اطبع(م.إحصائيات())`,exercise:`# أنشئ أداة تصفية سطور
صنف مصفّي
    باني(سطور)
        هذا.سطور = سطور
    نهاية
    دالة بالطول(حد_أدنى)
        متغير نتيجة = []
        # أكمل: أضف السطور الأطول من الحد
        ارجع نتيجة
    نهاية
نهاية`,hint:"استخدم طول(سطر) للتحقق من طول كل سطر.",tips:["اعمل على نسخة من البيانات","ارجع مصفوفة جديدة بدل التعديل","دعم تسلسل العمليات"],concepts:["معالجة نصوص","تصفية","إحصائيات"]},{id:"l5_45",title:"أداة البحث في النصوص",desc:"بناء محرك بحث بسيط مع ترتيب النتائج حسب الصلة.",explanation:"بناء محرك بحث يتطلب فهم الفهرسة والترتيب بالصلة. الفهرسة المسبقة تسرّع البحث — بدلاً من قراءة كل ملف كل مرة. الترتيب بالصلة: عد التطابقات، موقع الكلمة (العنوان أهم من المحتوى). البحث الجزئي (substring) مهم لتجربة المستخدم.",code:`# محرك بحث في مصفوفة نصوص
صنف محرك_بحث
    باني()
        هذا.وثائق = []
    نهاية
    دالة أضف(عنوان، محتوى)
        هذا.وثائق += [{"عنوان": عنوان، "محتوى": محتوى}]
    نهاية
    دالة ابحث(كلمة)
        متغير نتائج = []
        لكل وثيقة في هذا.وثائق
            نتائج += [وثيقة["عنوان"]]
        نهاية
        ارجع نتائج
    نهاية
نهاية

متغير م = جديد محرك_بحث()
م.أضف("درس 1"، "مقدمة في البرمجة")
م.أضف("درس 2"، "المتغيرات والأنواع")
اطبع(م.ابحث("برمجة"))`,exercise:`# أنشئ باحث مع عدّ التطابقات
صنف باحث
    باني(نصوص)
        هذا.نصوص = نصوص
    نهاية
    دالة ابحث(مصطلح)
        متغير نتائج = []
        لكل نص في هذا.نصوص
            نتائج += [نص]
        نهاية
        ارجع نتائج
    نهاية
نهاية`,hint:"قارن كل نص مع المصطلح وأضف المتطابقات فقط.",tips:["فهرسة البيانات تسرّع البحث","رتّب النتائج حسب الصلة","دعم البحث الجزئي"],concepts:["بحث نصي","فهرسة","ترتيب نتائج"]},{id:"l5_46",title:"محوّل البيانات",desc:"تحويل البيانات بين تنسيقات مختلفة مع حفظ السلامة.",explanation:"تحويل البيانات مهارة شائعة: JSON لـ YAML، CSV لـ JSON، XML لـ JSON. المحوّل يسجّل دوال التحويل بين الصيغ. التحويل المتسلسل: كم → ميل → قدم. تحقق من وجود التحويل وأعطِ خطأ واضحاً إن لم يوجد.",code:`# محوّل بين التنسيقات
صنف محوّل
    باني()
        هذا.تحويلات = {}
    نهاية
    دالة سجّل(من، إلى، دالة_تحويل)
        هذا.تحويلات[من + "_" + إلى] = دالة_تحويل
    نهاية
    دالة حوّل(من، إلى، قيمة)
        متغير مفتاح = من + "_" + إلى
        إذا (هذا.تحويلات[مفتاح] != لاشيء)
            ارجع هذا.تحويلات[مفتاح](قيمة)
        نهاية
        ارمي "لا يوجد تحويل من " + من + " إلى " + إلى
    نهاية
نهاية

متغير م = جديد محوّل()
م.سجّل("كم"، "ميل"، لامدا(ق) ارجع ق * 0.621 نهاية)
اطبع(م.حوّل("كم"، "ميل"، 10))`,exercise:`# أنشئ محوّل درجات حرارة
صنف حرارة
    ساكن دالة سلسيوس_فهرنهايت(د)
        ارجع د * 9 / 5 + 32
    نهاية
    ساكن دالة فهرنهايت_سلسيوس(د)
        # أكمل: المعادلة العكسية
    نهاية
نهاية
اطبع(حرارة.سلسيوس_فهرنهايت(100))`,hint:"المعادلة العكسية: (د - 32) * 5 / 9.",tips:["سجّل التحويلات كدوال مرنة","تحقق من وجود تحويل قبل التنفيذ","دعم التحويل المتسلسل"],concepts:["تحويل بيانات","دوال مرتبة أعلى","خرائط"]},{id:"l5_47",title:"تطبيق الحاسبة",desc:"بناء آلة حاسبة كاملة تدعم عمليات متعددة.",explanation:"الحاسبة مشروع كلاسيكي يجمع عدة مفاهيم: تحليل مدخلات، مطابقة أنماط، إدارة حالة. السجل (history) يتيح التراجع. الذاكرة (M+, MR, MC) ميزة مطلوبة. التحقق من القسمة على صفر ضروري. يمكن التوسع لدعم الأقواس والتعابير المركبة.",code:`# آلة حاسبة بسجل عمليات
صنف حاسبة
    باني()
        هذا.نتيجة = 0
        هذا.سجل = []
    نهاية
    دالة نفّذ(عملية، قيمة)
        طابق (عملية)
            عندما "+":
                هذا.نتيجة += قيمة
            عندما "-":
                هذا.نتيجة -= قيمة
            عندما "*":
                هذا.نتيجة *= قيمة
            عندما "/":
                إذا (قيمة != 0)
                    هذا.نتيجة /= قيمة
                نهاية
        نهاية
        هذا.سجل += [عملية + إلى_نص(قيمة)]
    نهاية
نهاية

متغير ح = جديد حاسبة()
ح.نفّذ("+"، 10)
ح.نفّذ("*"، 3)
اطبع(ح.نتيجة)`,exercise:`# أضف دعم الذاكرة للحاسبة
صنف حاسبة_ذاكرة
    باني()
        هذا.نتيجة = 0
        هذا.ذاكرة = 0
    نهاية
    دالة حفظ()
        هذا.ذاكرة = هذا.نتيجة
    نهاية
    # أكمل: دالة استرجع() ودالة امسح_ذاكرة()
نهاية`,hint:"استرجع تنسخ الذاكرة للنتيجة، وامسح تصفّر الذاكرة.",tips:["أضف سجل لتتبع العمليات","تحقق من القسمة على صفر","دعم التراجع عن آخر عملية"],concepts:["آلة حاسبة","مطابقة أنماط","سجل عمليات"]},{id:"l5_48",title:"مدير المهام",desc:"بناء تطبيق لإدارة المهام مع أولويات وحالات.",explanation:"مدير المهام مشروع عملي يعلّم التصفية والإدارة. الحالات الواضحة: جديدة/قيد العمل/مكتملة. الأولويات للترتيب. المعرّف الفريد (auto-increment) لكل مهمة. التاريخ لتتبع متى أُنشئت وأُكملت. يمكن الحفظ لملف JSON للاستمرارية.",code:`# مدير مهام مع أولويات
صنف مدير_مهام
    باني()
        هذا.مهام = []
        هذا.عدّاد = 0
    نهاية
    دالة أضف(عنوان، أولوية)
        هذا.عدّاد += 1
        هذا.مهام += [{"رقم": هذا.عدّاد، "عنوان": عنوان، "أولوية": أولوية، "حالة": "جديدة"}]
    نهاية
    دالة أكمل(رقم)
        لكل مهمة في هذا.مهام
            إذا (مهمة["رقم"] == رقم)
                مهمة["حالة"] = "مكتملة"
            نهاية
        نهاية
    نهاية
    دالة المعلّقة()
        متغير نتيجة = []
        لكل مهمة في هذا.مهام
            إذا (مهمة["حالة"] != "مكتملة")
                نتيجة += [مهمة]
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير م = جديد مدير_مهام()
م.أضف("كتابة تقرير"، "عالية")
م.أضف("مراجعة كود"، "متوسطة")
اطبع(م.المعلّقة())`,exercise:`# أضف تصفية حسب الأولوية
صنف مهام
    باني()
        هذا.قائمة = []
    نهاية
    دالة أضف(عنوان، أولوية)
        هذا.قائمة += [{"عنوان": عنوان، "أولوية": أولوية}]
    نهاية
    # أكمل: دالة حسب_أولوية(مستوى) تُرجع المطابقة
نهاية`,hint:'كرر على القائمة وقارن مهمة["أولوية"] == مستوى.',tips:["استخدم حالات واضحة (جديدة، قيد العمل، مكتملة)","الأولويات تساعد في الترتيب","أضف تاريخ الإنشاء لكل مهمة"],concepts:["إدارة مهام","تصفية","حالات"]},{id:"l5_49",title:"مولّد التقارير",desc:"إنشاء تقارير منسقة من بيانات مهيكلة.",explanation:"توليد التقارير يجمع بين البيانات والتنسيق. الفصل مهم: اجمع البيانات أولاً، ثم نسّقها. الأقسام تنظّم المحتوى. الإحصائيات التلقائية (مجموع، متوسط) تفيد. دعم عدة صيغ إخراج (text, HTML, CSV) يزيد المرونة.",code:`# مولّد تقارير نصية
صنف تقرير
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.أقسام = []
    نهاية
    دالة أضف_قسم(عنوان، محتوى)
        هذا.أقسام += [{"عنوان": عنوان، "محتوى": محتوى}]
    نهاية
    دالة اطبع_تقرير()
        اطبع_سطر("╔══ " + هذا.عنوان + " ══╗")
        لكل قسم في هذا.أقسام
            اطبع_سطر("► " + قسم["عنوان"])
            اطبع_سطر("  " + قسم["محتوى"])
        نهاية
        اطبع_سطر("╚══════════════════╝")
    نهاية
نهاية

متغير ت = جديد تقرير("تقرير المبيعات")
ت.أضف_قسم("الإجمالي"، "5000 ريال")
ت.أضف_قسم("الهدف"، "مكتمل 85%")
ت.اطبع_تقرير()`,exercise:`# أنشئ تقرير مع إحصائيات
صنف تقرير_إحصائي
    باني(عنوان، أرقام)
        هذا.عنوان = عنوان
        هذا.أرقام = أرقام
    نهاية
    # أكمل: دالة ملخص() تطبع المجموع والمتوسط
نهاية
متغير ت = جديد تقرير_إحصائي("مبيعات"، [100، 200، 150])`,hint:"احسب المجموع بحلقة، ثم اقسم على طول(أرقام) للمتوسط.",tips:["نسّق الإخراج بشكل مقروء","فصل البيانات عن التنسيق","دعم أنواع تقارير مختلفة"],concepts:["توليد تقارير","تنسيق","إحصائيات"]},{id:"l5_50",title:"إطار عمل CLI متكامل",desc:"تجميع كل المفاهيم في إطار كامل لبناء تطبيقات سطر أوامر.",explanation:"إطار CLI المتكامل يجمع: تحليل الأوامر، مساعدة تلقائية (--help)، أوامر فرعية، والتحقق. مثل git: أمر رئيسي مع أوامر فرعية (commit, push, pull). التوسعية: أضف أوامر جديدة بدون تعديل النواة. هذا نمط Command من Design Patterns.",code:`# إطار CLI متكامل
صنف تطبيق_CLI
    باني(اسم، إصدار)
        هذا.اسم = اسم
        هذا.إصدار = إصدار
        هذا.أوامر = {}
    نهاية
    دالة أمر(اسم، وصف، معالج)
        هذا.أوامر[اسم] = {"وصف": وصف، "معالج": معالج}
    نهاية
    دالة مساعدة()
        اطبع_سطر(هذا.اسم + " v" + هذا.إصدار)
        لكل اسم في هذا.أوامر
            اطبع_سطر("  " + اسم + " — " + هذا.أوامر[اسم]["وصف"])
        نهاية
    نهاية
    دالة شغّل(اسم_أمر)
        إذا (هذا.أوامر[اسم_أمر] != لاشيء)
            هذا.أوامر[اسم_أمر]["معالج"]()
        وإلا
            اطبع_سطر("أمر غير معروف: " + اسم_أمر)
        نهاية
    نهاية
نهاية

متغير تطبيق = جديد تطبيق_CLI("أداتي"، "1.0")
تطبيق.أمر("مساعدة"، "عرض المساعدة"، لامدا() تطبيق.مساعدة() نهاية)
تطبيق.أمر("إصدار"، "رقم الإصدار"، لامدا() اطبع(تطبيق.إصدار) نهاية)
تطبيق.شغّل("مساعدة")`,exercise:`# وسّع الإطار بدعم الأوامر الفرعية
صنف أمر_فرعي
    باني(اسم)
        هذا.اسم = اسم
        هذا.أفعال = {}
    نهاية
    دالة فعل(اسم، معالج)
        هذا.أفعال[اسم] = معالج
    نهاية
    # أكمل: دالة نفّذ(اسم_فعل)
نهاية

متغير أ = جديد أمر_فرعي("ملف")
أ.فعل("أنشئ"، لامدا() اطبع("إنشاء ملف") نهاية)`,hint:"تحقق من وجود الفعل في هذا.أفعال ثم استدعِه.",tips:["صمم إطاراً قابلاً للتوسيع","أضف دعم المساعدة التلقائي","اجعل الأوامر قابلة للتركيب"],concepts:["إطار عمل","نمط الأمر","تصميم API"]}]},Ec={id:"l5_data_processing",title:"معالجة البيانات",icon:"📊",lessons:[{id:"l5_51",title:"تحليل بيانات CSV",desc:"قراءة وتحليل بيانات بتنسيق CSV وتحويلها لهياكل بيانات.",explanation:'CSV من أكثر صيغ البيانات استخداماً — Excel وقواعد البيانات تصدّرها. التحليل يحوّل الأسطر لكائنات بأسماء أعمدة كمفاتيح. التحديات: الفواصل داخل القيم (escaped by quotes)، الأسطر الناقصة، ترميز الأحرف (UTF-8 vs Latin1). التحويل لخرائط يسهّل الوصول row["name"] بدلاً من row[2].',code:`# محلل CSV بسيط
صنف محلل_CSV
    باني(فاصل)
        هذا.فاصل = فاصل
        هذا.رؤوس = []
        هذا.صفوف = []
    نهاية
    دالة حمّل(رؤوس، صفوف)
        هذا.رؤوس = رؤوس
        هذا.صفوف = صفوف
    نهاية
    دالة عدد_صفوف()
        ارجع طول(هذا.صفوف)
    نهاية
    دالة صف(رقم)
        متغير نتيجة = {}
        لكل ت في نطاق(طول(هذا.رؤوس))
            نتيجة[هذا.رؤوس[ت]] = هذا.صفوف[رقم][ت]
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير م = جديد محلل_CSV("،")
م.حمّل(["اسم"، "عمر"]، [["أحمد"، 25]، ["سارة"، 30]])
اطبع(م.صف(0))`,exercise:`# أنشئ محلل يدعم التصفية
صنف بيانات
    باني(رؤوس، صفوف)
        هذا.رؤوس = رؤوس
        هذا.صفوف = صفوف
    نهاية
    دالة عمود(اسم)
        متغير ت = 0
        متغير قيم = []
        # أكمل: ابحث عن فهرس العمود واجمع قيمه
        ارجع قيم
    نهاية
نهاية`,hint:"ابحث عن فهرس الاسم في الرؤوس ثم اجمع صفوف[*][فهرس].",tips:["حوّل الصفوف لخرائط للوصول بالاسم","تحقق من تساوي عدد الأعمدة","ادعم الفواصل المختلفة"],concepts:["تحليل CSV","تحويل بيانات","فهرسة"]},{id:"l5_52",title:"تصفية وترتيب البيانات",desc:"تطبيق مرشحات متعددة وترتيب النتائج حسب معايير.",explanation:'التصفية والترتيب عمليات يومية على البيانات. سلسلة العمليات (method chaining) تجعل الكود قابلاً للقراءة: data.where("active", true).orderBy("date"). النسخ بدلاً من التعديل يحافظ على البيانات الأصلية. هذا النمط مستخدم في ORMs مثل Django QuerySet وEloquent.',code:`# نظام تصفية مرن
صنف مرشّح
    باني(بيانات)
        هذا.بيانات = بيانات
        هذا.نتائج = بيانات
    نهاية
    دالة حيث(مفتاح، قيمة)
        متغير مصفّى = []
        لكل عنصر في هذا.نتائج
            إذا (عنصر[مفتاح] == قيمة)
                مصفّى += [عنصر]
            نهاية
        نهاية
        هذا.نتائج = مصفّى
        ارجع هذا
    نهاية
    دالة احصل()
        ارجع هذا.نتائج
    نهاية
نهاية

متغير طلاب = [{"اسم": "أحمد"، "مستوى": 5}، {"اسم": "سارة"، "مستوى": 5}، {"اسم": "خالد"، "مستوى": 3}]
متغير م = جديد مرشّح(طلاب)
اطبع(م.حيث("مستوى"، 5).احصل())`,exercise:`# أنشئ مرشح بعدة شروط
صنف فلتر
    باني(قائمة)
        هذا.قائمة = قائمة
    نهاية
    دالة أكبر_من(مفتاح، قيمة)
        متغير نتيجة = []
        لكل عنصر في هذا.قائمة
            # أكمل: أضف إذا عنصر[مفتاح] > قيمة
        نهاية
        ارجع نتيجة
    نهاية
نهاية`,hint:"قارن عنصر[مفتاح] > قيمة ثم أضف للنتيجة.",tips:["أرجع هذا لتسلسل العمليات","انسخ البيانات قبل التصفية","دعم عدة أنواع مقارنة"],concepts:["تصفية بيانات","تسلسل عمليات","مقارنة"]},{id:"l5_53",title:"التجميع والتلخيص",desc:"تجميع البيانات حسب فئات وحساب إحصائيات لكل مجموعة.",explanation:"التجميع (GROUP BY في SQL) من أهم عمليات تحليل البيانات: المبيعات حسب الشهر، المستخدمين حسب البلد. الدوال التجميعية: مجموع، عدد، متوسط، أقل، أكبر. في Python، pandas.groupby يفعل هذا. النتيجة: خريطة من فئة لقيمة مجمّعة.",code:`# تجميع بيانات حسب فئة
دالة جمّع_حسب(بيانات، مفتاح)
    متغير مجموعات = {}
    لكل عنصر في بيانات
        متغير ف = عنصر[مفتاح]
        إذا (مجموعات[ف] == لاشيء)
            مجموعات[ف] = []
        نهاية
        مجموعات[ف] += [عنصر]
    نهاية
    ارجع مجموعات
نهاية

متغير مبيعات = [
    {"قسم": "أ"، "مبلغ": 100}،
    {"قسم": "ب"، "مبلغ": 200}،
    {"قسم": "أ"، "مبلغ": 150}
]
متغير نتيجة = جمّع_حسب(مبيعات، "قسم")
اطبع(نتيجة)`,exercise:`# أنشئ دالة تجميع مع حساب المجموع
دالة مجموع_حسب(بيانات، مفتاح_تجميع، مفتاح_قيمة)
    متغير مجاميع = {}
    لكل عنصر في بيانات
        متغير ف = عنصر[مفتاح_تجميع]
        # أكمل: أضف القيمة للمجموعة
    نهاية
    ارجع مجاميع
نهاية

متغير ب = [{"نوع": "أ"، "كمية": 5}، {"نوع": "أ"، "كمية": 3}]
اطبع(مجموع_حسب(ب، "نوع"، "كمية"))`,hint:"تحقق إذا المجموعة موجودة، ثم أضف عنصر[مفتاح_قيمة].",tips:["ابدأ المجموعة بصفر عند أول ظهور","دعم عدة دوال تجميع (مجموع، عدد، متوسط)","النتيجة خريطة من فئة لقيمة"],concepts:["تجميع بيانات","تلخيص","خرائط"]},{id:"l5_54",title:"تحويل البيانات",desc:"تطبيق تحويلات على هياكل البيانات مع الحفاظ على الشكل.",explanation:"خط أنابيب التحويل (Transformation Pipeline) يمرّر البيانات عبر خطوات متتالية. كل خطوة دالة نقية: مدخل معروف، مخرج معروف، بدون تأثيرات جانبية. إرجاع this يتيح سلسلة العمليات. هذا النمط في كل مكان: RxJS، D3.js، Apache Spark.",code:`# تحويل بيانات مع خط أنابيب
صنف خط_تحويل
    باني(بيانات)
        هذا.بيانات = بيانات
    نهاية
    دالة طبّق(دالة_تحويل)
        متغير نتيجة = []
        لكل عنصر في هذا.بيانات
            نتيجة += [دالة_تحويل(عنصر)]
        نهاية
        هذا.بيانات = نتيجة
        ارجع هذا
    نهاية
    دالة نتيجة()
        ارجع هذا.بيانات
    نهاية
نهاية

متغير أرقام = [1، 2، 3، 4، 5]
متغير خ = جديد خط_تحويل(أرقام)
متغير ن = خ.طبّق(لامدا(ق) ارجع ق * 2 نهاية).نتيجة()
اطبع(ن)`,exercise:`# أنشئ خط تحويل مع تصفية
صنف معالج
    باني(بيانات)
        هذا.بيانات = بيانات
    نهاية
    دالة حوّل(دالة_م)
        متغير نتيجة = []
        لكل ع في هذا.بيانات
            نتيجة += [دالة_م(ع)]
        نهاية
        هذا.بيانات = نتيجة
        ارجع هذا
    نهاية
    # أكمل: دالة صفّي(دالة_شرط) تبقي المتحقق فقط
نهاية`,hint:"صفّي تستدعي دالة_شرط لكل عنصر وتبقي الصحيح فقط.",tips:["أرجع هذا من كل عملية","كل خطوة تنتج بيانات جديدة","استخدم لامدا للمرونة"],concepts:["تحويل بيانات","خط أنابيب","لامدا"]},{id:"l5_55",title:"الإحصائيات الوصفية",desc:"حساب المقاييس الإحصائية الأساسية لمجموعات بيانات.",code:`# حاسبة إحصائيات
صنف إحصائيات
    باني(أرقام)
        هذا.أرقام = أرقام
    نهاية
    دالة مجموع()
        متغير م = 0
        لكل ر في هذا.أرقام
            م += ر
        نهاية
        ارجع م
    نهاية
    دالة متوسط()
        ارجع هذا.مجموع() / طول(هذا.أرقام)
    نهاية
    دالة أكبر_قيمة()
        متغير أ = هذا.أرقام[0]
        لكل ر في هذا.أرقام
            إذا (ر > أ)
                أ = ر
            نهاية
        نهاية
        ارجع أ
    نهاية
نهاية

متغير إ = جديد إحصائيات([10، 20، 30، 40، 50])
اطبع("المتوسط: " + إلى_نص(إ.متوسط()))`,exercise:`# أضف حساب المدى والانحراف
صنف تحليل
    باني(قيم)
        هذا.قيم = قيم
    نهاية
    دالة مدى()
        # أكمل: الفرق بين أكبر وأصغر قيمة
    نهاية
    دالة عدد()
        ارجع طول(هذا.قيم)
    نهاية
نهاية
متغير ت = جديد تحليل([5، 10، 15، 20])
اطبع(ت.مدى())`,hint:"المدى = أكبر قيمة - أصغر قيمة. استخدم حلقة لإيجادهما.",tips:["تحقق من وجود بيانات قبل الحساب","كل مقياس دالة مستقلة","استخدم دوال مساعدة لتجنب التكرار"],concepts:["إحصائيات","مقاييس","تحليل بيانات"]},{id:"l5_56",title:"التحقق من البيانات",desc:"بناء خط أنابيب للتحقق من صحة البيانات المدخلة.",explanation:'التحقق من البيانات حاسم لجودة التحليل: "Garbage In, Garbage Out". القواعد المسلسلة تتيح المرونة. اجمع كل الأخطاء بدل التوقف عند أول خطأ — المستخدم يرى كل المشاكل مرة واحدة. التحقق من الأنواع والنطاقات والصيغ (بريد، هاتف) ضروري.',code:`# نظام تحقق متسلسل
صنف متحقق
    باني()
        هذا.قواعد = []
        هذا.أخطاء = []
    نهاية
    دالة أضف_قاعدة(اسم، دالة_فحص)
        هذا.قواعد += [{"اسم": اسم، "فحص": دالة_فحص}]
    نهاية
    دالة تحقق(قيمة)
        هذا.أخطاء = []
        لكل قاعدة في هذا.قواعد
            إذا (قاعدة["فحص"](قيمة) == خطأ)
                هذا.أخطاء += [قاعدة["اسم"]]
            نهاية
        نهاية
        ارجع طول(هذا.أخطاء) == 0
    نهاية
نهاية

متغير م = جديد متحقق()
م.أضف_قاعدة("موجب"، لامدا(ق) ارجع ق > 0 نهاية)
م.أضف_قاعدة("أقل من 100"، لامدا(ق) ارجع ق < 100 نهاية)
اطبع(م.تحقق(50))
اطبع(م.تحقق(-5))`,exercise:`# أنشئ متحقق للنصوص
صنف متحقق_نص
    باني()
        هذا.أخطاء = []
    نهاية
    دالة تحقق_طول(نص، حد_أدنى، حد_أقصى)
        متغير ط = طول(نص)
        إذا (ط < حد_أدنى)
            هذا.أخطاء += ["قصير جداً"]
        نهاية
        # أكمل: تحقق من الحد الأقصى
        ارجع طول(هذا.أخطاء) == 0
    نهاية
نهاية`,hint:"أضف شرط إذا (ط > حد_أقصى) لفحص الحد الأعلى.",tips:["اجمع كل الأخطاء بدل التوقف عند أول خطأ","أرجع صحيح/خطأ مع قائمة أخطاء","اجعل القواعد قابلة للتركيب"],concepts:["تحقق بيانات","خط أنابيب","قواعد"]},{id:"l5_57",title:"تنسيق البيانات للعرض",desc:"تحويل البيانات الخام لتنسيقات مقروءة ومنظمة.",explanation:"تنسيق العرض يحوّل البيانات الخام لمخرجات مقروءة. الجداول النصية تحتاج محاذاة الأعمدة. فواصل tab (Ŝt) للبساطة، لكن العرض الثابت أجمل. الترقيم للقوائم يسهّل القراءة. دعم عدة صيغ (text, JSON, CSV, HTML) يزيد المرونة.",code:`# منسّق جداول نصية
صنف جدول
    باني(أعمدة)
        هذا.أعمدة = أعمدة
        هذا.صفوف = []
    نهاية
    دالة أضف(صف)
        هذا.صفوف += [صف]
    نهاية
    دالة اعرض()
        متغير رأس = ""
        لكل عمود في هذا.أعمدة
            رأس += عمود + "	"
        نهاية
        اطبع_سطر(رأس)
        لكل صف في هذا.صفوف
            متغير سطر = ""
            لكل قيمة في صف
                سطر += إلى_نص(قيمة) + "	"
            نهاية
            اطبع_سطر(سطر)
        نهاية
    نهاية
نهاية

متغير ج = جديد جدول(["الاسم"، "العمر"])
ج.أضف(["أحمد"، 25])
ج.اعرض()`,exercise:`# أنشئ منسّق قوائم مرقمة
صنف منسّق_قائمة
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.عناصر = []
    نهاية
    دالة أضف(عنصر)
        هذا.عناصر += [عنصر]
    نهاية
    # أكمل: دالة اعرض() تطبع العنوان ثم العناصر مرقمة
نهاية`,hint:"استخدم نطاق(طول(هذا.عناصر)) للترقيم بداية من 1.",tips:["أضف حدود وفواصل بصرية","نسّق الأرقام بمحاذاة","ادعم عدة تنسيقات إخراج"],concepts:["تنسيق بيانات","عرض جداول","تنسيق نصي"]},{id:"l5_58",title:"بناء التقارير المركبة",desc:"دمج مصادر بيانات متعددة في تقرير واحد شامل.",explanation:"التقارير المركبة تجمع بيانات من مصادر متعددة لصورة شاملة. نمط Builder يجعل الإنشاء مرناً: .addSection().addChart().addSummary(). سلسلة الإضافات تجعل الكود واضحاً. الفصل بين جمع البيانات والتنسيق يتيح إعادة الاستخدام.",code:`# باني تقارير مركبة
صنف باني_تقرير
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.أجزاء = []
    نهاية
    دالة أضف_نص(نص)
        هذا.أجزاء += [{"نوع": "نص"، "محتوى": نص}]
        ارجع هذا
    نهاية
    دالة ابنِ()
        اطبع_سطر("=== " + هذا.عنوان + " ===")
        لكل جزء في هذا.أجزاء
            اطبع_سطر(جزء["محتوى"])
        نهاية
    نهاية
نهاية

متغير ب = جديد باني_تقرير("تقرير")
ب.أضف_نص("المبيعات: 1000").أضف_نص("الهدف: مكتمل").ابنِ()`,exercise:`# أنشئ تقرير مقارنة
دالة قارن(فترة1، فترة2)
    متغير مج1 = 0
    متغير مج2 = 0
    لكل ق في فترة1
        مج1 += ق
    نهاية
    # أكمل: احسب مج2 واطبع الفرق
نهاية
قارن([100، 200]، [150، 250])`,hint:"اطبع مج1 ومج2 ثم الفرق (مج2 - مج1) والنسبة.",tips:["فصل جمع البيانات عن التنسيق","دعم تسلسل الإضافات","أضف إحصائيات ملخصة تلقائياً"],concepts:["تقارير مركبة","تسلسل بناء","دمج بيانات"]},{id:"l5_59",title:"المعالجة الدفعية",desc:"معالجة كميات كبيرة من البيانات على دفعات مع تتبع التقدم.",explanation:"المعالجة الدفعية ضرورية للبيانات الضخمة — لا يمكن تحميل مليون سجل في الذاكرة دفعة واحدة. الدفعات (مثلاً 1000 سجل) تحل المشكلة. تتبع التقدم مهم للمعالجة الطويلة. معالجة الأخطاء: تابع مع العناصر الفاشلة بدلاً من إيقاف كل شيء. إعادة المحاولة (retry) للأخطاء المؤقتة.",code:`# معالج دفعي مع تتبع
صنف معالج_دفعي
    باني(حجم_دفعة)
        هذا.حجم = حجم_دفعة
        هذا.معالَج = 0
        هذا.أخطاء = 0
    نهاية
    دالة عالج(بيانات، دالة_معالجة)
        متغير نتائج = []
        لكل عنصر في بيانات
            حاول
                نتائج += [دالة_معالجة(عنصر)]
                هذا.معالَج += 1
            امسك (خطأ)
                هذا.أخطاء += 1
            نهاية
        نهاية
        ارجع نتائج
    نهاية
    دالة تقرير()
        اطبع_سطر("تم معالجة: " + إلى_نص(هذا.معالَج))
        اطبع_سطر("أخطاء: " + إلى_نص(هذا.أخطاء))
    نهاية
نهاية

متغير م = جديد معالج_دفعي(10)
متغير ن = م.عالج([1، 2، 3، 4، 5]، لامدا(ق) ارجع ق * 10 نهاية)
م.تقرير()
اطبع(ن)`,exercise:`# أنشئ معالج مع إعادة المحاولة
صنف معالج_آمن
    باني(محاولات)
        هذا.محاولات = محاولات
    نهاية
    دالة نفّذ(دالة_عمل)
        متغير م = 0
        بينما (م < هذا.محاولات)
            حاول
                ارجع دالة_عمل()
            امسك (خطأ)
                م += 1
            نهاية
        نهاية
        ارجع لاشيء
    نهاية
نهاية`,hint:"الحلقة تستمر حتى النجاح أو نفاد المحاولات.",tips:["حدد حجم الدفعة المناسب","تعامل مع الأخطاء دون توقف","سجّل التقدم للمتابعة"],concepts:["معالجة دفعية","معالجة أخطاء","تتبع تقدم"]},{id:"l5_60",title:"أنماط ETL",desc:"تطبيق نمط الاستخراج-التحويل-التحميل لمعالجة البيانات.",explanation:"ETL (Extract-Transform-Load) معيار الصناعة لنقل البيانات: استخرج من المصدر، حوّل للصيغة المطلوبة، حمّل للوجهة. كل خطوة مستقلة وقابلة للاختبار. التسجيل (logging) في كل مرحلة للتتبع والتصحيح. أدوات مثل Apache Airflow وdbt متخصصة في هذا.",code:`# خط أنابيب ETL
صنف خط_ETL
    باني()
        هذا.محوّلات = []
    نهاية
    دالة حوّل(دالة_تحويل)
        هذا.محوّلات += [دالة_تحويل]
        ارجع هذا
    نهاية
    دالة شغّل(بيانات)
        لكل تحويل في هذا.محوّلات
            بيانات = تحويل(بيانات)
        نهاية
        ارجع بيانات
    نهاية
نهاية

متغير خ = جديد خط_ETL()
خ.حوّل(لامدا(ب)
    متغير ن = []
    لكل ق في ب
        ن += [ق * 2]
    نهاية
    ارجع ن
نهاية)
اطبع(خ.شغّل([1، 2، 3، 4، 5]))`,exercise:`# أنشئ ETL مع تسجيل
صنف خط_بيانات
    باني(اسم)
        هذا.اسم = اسم
        هذا.سجل = []
    نهاية
    دالة خطوة(وصف، دالة_عمل)
        هذا.سجل += [وصف]
        ارجع هذا
    نهاية
    # أكمل: دالة نفّذ(بيانات) تطبّق الخطوات
نهاية`,hint:"خزّن الدوال مع الأوصاف ثم نفّذها بالتسلسل.",tips:["كل خطوة مستقلة وقابلة للاختبار","سجّل كل مرحلة للتتبع","دعم تسلسل العمليات"],concepts:["ETL","خط أنابيب","تسلسل عمليات"]}]},qc={id:"l5_testing_quality",title:"اختبار وجودة الكود",icon:"🧪",lessons:[{id:"l5_61",title:"أنماط الاختبار الوحدوي",desc:"كتابة اختبارات وحدوية منظمة للتحقق من صحة الدوال.",explanation:"الاختبار الوحدوي يفحص وحدة واحدة (دالة/صنف) بمعزل عن الباقي. نمط AAA: Arrange (إعداد)، Act (تنفيذ)، Assert (تأكيد). كل اختبار يفحص شيئاً واحداً — اسمه يصف ما يختبر. التغطية العالية تعطي ثقة للتعديل. أطر مثل Jest وPyTest توفر بنية تحتية قوية.",code:`# إطار اختبار بسيط
صنف اختبار
    باني(اسم)
        هذا.اسم = اسم
        هذا.نجح = 0
        هذا.فشل = 0
    نهاية
    دالة تأكد_يساوي(فعلي، متوقع، وصف)
        إذا (فعلي == متوقع)
            هذا.نجح += 1
            اطبع_سطر("✓ " + وصف)
        وإلا
            هذا.فشل += 1
            اطبع_سطر("✗ " + وصف)
        نهاية
    نهاية
    دالة ملخص()
        اطبع_سطر("--- " + هذا.اسم + " ---")
        اطبع_سطر("نجح: " + إلى_نص(هذا.نجح) + " فشل: " + إلى_نص(هذا.فشل))
    نهاية
نهاية

# اختبار دالة الجمع
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

متغير ت = جديد اختبار("اختبار الجمع")
ت.تأكد_يساوي(جمع(2، 3)، 5، "2+3=5")
ت.تأكد_يساوي(جمع(0، 0)، 0، "0+0=0")
ت.تأكد_يساوي(جمع(-1، 1)، 0، "-1+1=0")
ت.ملخص()`,exercise:`# أنشئ اختبارات لدالة المضاعفة
دالة ضاعف(ن)
    ارجع ن * 2
نهاية

صنف فحص
    باني()
        هذا.نتائج = []
    نهاية
    دالة تأكد(شرط، وصف)
        هذا.نتائج += [{"نجح": شرط، "وصف": وصف}]
    نهاية
    # أكمل: دالة اطبع_نتائج()
نهاية`,hint:"كرر على هذا.نتائج واطبع ✓ أو ✗ حسب نجح.",tips:["اختبر حالة واحدة لكل تأكيد","سمِّ الاختبارات بوضوح","اختبر القيم الحدية"],concepts:["اختبار وحدوي","تأكيدات","إطار اختبار"]},{id:"l5_62",title:"التأكيدات المتقدمة",desc:"بناء مكتبة تأكيدات غنية تدعم أنواعاً متعددة من المقارنات.",explanation:'التأكيدات assertions هي لغة الاختبارات: assertEquals، assertTrue، assertThrows، assertContains. رسائل الخطأ الواضحة ("expected 5, got 3") تسرّع التصحيح. التأكيدات المخصصة لمجالك (assertValidEmail) تجعل الاختبارات أوضح. مكتبات مثل Chai توفر أسلوب fluent: expect(x).to.equal(5).',code:`# مكتبة تأكيدات متقدمة
صنف تأكيدات
    ساكن دالة يساوي(أ، ب، رسالة)
        إذا (أ == ب)
            اطبع_سطر("✓ " + رسالة)
        وإلا
            اطبع_سطر("✗ " + رسالة + " (توقع: " + إلى_نص(ب) + " حصل: " + إلى_نص(أ) + ")")
        نهاية
    نهاية
    ساكن دالة أكبر(أ، ب، رسالة)
        إذا (أ > ب)
            اطبع_سطر("✓ " + رسالة)
        وإلا
            اطبع_سطر("✗ " + رسالة)
        نهاية
    نهاية
نهاية

تأكيدات.يساوي(5 + 3، 8، "الجمع صحيح")
تأكيدات.أكبر(10، 5، "10 أكبر من 5")`,exercise:`# أضف تأكيدات للمصفوفات
صنف فحص_مصفوفة
    ساكن دالة تحتوي(مصفوفة، قيمة، رسالة)
        متغير موجود = خطأ
        لكل ع في مصفوفة
            إذا (ع == قيمة)
                موجود = صحيح
            نهاية
        نهاية
        # أكمل: اطبع النتيجة حسب موجود
    نهاية
نهاية
فحص_مصفوفة.تحتوي([1، 2، 3]، 2، "تحتوي 2")`,hint:"إذا (موجود) اطبع ✓ وإلا اطبع ✗ مع الرسالة.",tips:["رسائل الخطأ توضح المتوقع والفعلي","تأكيدات ساكنة لا تحتاج كائناً","ادعم مقارنة أنواع مختلفة"],concepts:["تأكيدات متقدمة","دوال ساكنة","مقارنات"]},{id:"l5_63",title:"تنظيم مجموعات الاختبار",desc:"تنظيم الاختبارات في مجموعات ذات هيكل واضح وقابل للتوسيع.",explanation:"التنظيم ضروري عندما تكبر الاختبارات: مجموعات حسب الميزة أو الوحدة. beforeEach/afterEach للإعداد والتنظيف. describe/it هيكل شائع للوصف. شغّل مجموعة واحدة أثناء التطوير، والكل قبل النشر. الملخص النهائي يعطي صورة سريعة.",code:`# تنظيم اختبارات في مجموعات
صنف مجموعة_اختبار
    باني(اسم)
        هذا.اسم = اسم
        هذا.اختبارات = []
    نهاية
    دالة أضف(وصف، دالة_اختبار)
        هذا.اختبارات += [{"وصف": وصف، "دالة": دالة_اختبار}]
    نهاية
    دالة شغّل()
        اطبع_سطر("═══ " + هذا.اسم + " ═══")
        متغير نجح = 0
        لكل اختبار في هذا.اختبارات
            حاول
                اختبار["دالة"]()
                نجح += 1
                اطبع_سطر("  ✓ " + اختبار["وصف"])
            امسك (خطأ)
                اطبع_سطر("  ✗ " + اختبار["وصف"])
            نهاية
        نهاية
        اطبع_سطر("النتيجة: " + إلى_نص(نجح) + "/" + إلى_نص(طول(هذا.اختبارات)))
    نهاية
نهاية

متغير م = جديد مجموعة_اختبار("الرياضيات")
م.أضف("الجمع"، لامدا()
    إذا (2 + 2 != 4)
        ارمي "خطأ في الجمع"
    نهاية
نهاية)
م.أضف("الضرب"، لامدا()
    إذا (3 * 4 != 12)
        ارمي "خطأ في الضرب"
    نهاية
نهاية)
م.شغّل()`,exercise:`# أنشئ مشغّل اختبارات متعدد المجموعات
صنف مشغّل
    باني()
        هذا.مجموعات = []
    نهاية
    دالة أضف_مجموعة(اسم، اختبارات)
        هذا.مجموعات += [{"اسم": اسم، "اختبارات": اختبارات}]
    نهاية
    # أكمل: دالة شغّل_الكل() تشغل كل المجموعات
نهاية`,hint:"كرر على المجموعات ثم على اختبارات كل مجموعة.",tips:["جمّع الاختبارات المتعلقة معاً","كل مجموعة مستقلة عن الأخرى","اطبع ملخص نهائي شامل"],concepts:["مجموعات اختبار","تنظيم","حاول/امسك"]},{id:"l5_64",title:"المحاكاة في الاختبارات",desc:"استخدام كائنات محاكاة لعزل الوحدات قيد الاختبار.",explanation:'المحاكاة (Mocking) تعزل الوحدة عن تبعياتها الخارجية (API، DB، ملفات). الأنواع: Stub (إرجاع قيم ثابتة)، Spy (تسجيل الاستدعاءات)، Mock (تحديد توقعات). verify(استُدعي "save" مرة) يتحقق من السلوك. مكتبات مثل Sinon وMockito متخصصة.',code:`# محاكاة خدمة خارجية
صنف خدمة_وهمية
    باني()
        هذا.استدعاءات = []
        هذا.ردود = {}
    نهاية
    دالة عند_استدعاء(اسم، نتيجة)
        هذا.ردود[اسم] = نتيجة
    نهاية
    دالة استدعِ(اسم)
        هذا.استدعاءات += [اسم]
        ارجع هذا.ردود[اسم]
    نهاية
    دالة تم_استدعاء(اسم)
        لكل س في هذا.استدعاءات
            إذا (س == اسم)
                ارجع صحيح
            نهاية
        نهاية
        ارجع خطأ
    نهاية
نهاية

متغير خ = جديد خدمة_وهمية()
خ.عند_استدعاء("بيانات"، [1، 2، 3])
اطبع(خ.استدعِ("بيانات"))
اطبع(خ.تم_استدعاء("بيانات"))`,exercise:`# أنشئ عدّاد استدعاءات
صنف عدّاد
    باني()
        هذا.سجل = {}
    نهاية
    دالة سجّل(اسم)
        إذا (هذا.سجل[اسم] == لاشيء)
            هذا.سجل[اسم] = 0
        نهاية
        هذا.سجل[اسم] += 1
    نهاية
    # أكمل: دالة عدد(اسم) تُرجع المرات
نهاية`,hint:"ارجع هذا.سجل[اسم] أو 0 إذا لم يكن موجوداً.",tips:["المحاكاة تعزل الاختبار عن التبعيات","سجّل كل استدعاء للتحقق لاحقاً","حدد الردود المتوقعة مسبقاً"],concepts:["محاكاة","عزل اختبارات","تسجيل استدعاءات"]},{id:"l5_65",title:"اختبار الحالات الحدية",desc:"تحديد واختبار الحالات الحدية والاستثنائية بشكل منهجي.",explanation:"الحالات الحدية (Edge Cases) حيث الأخطاء تختبئ: الصفر، الفارغ، الأقصى، null، السالب. لكل دالة، اسأل: ماذا لو القائمة فارغة؟ ماذا لو الرقم سالب؟ التغطية: قيمة عادية، قيمة حدية، قيمة خاطئة. assertThrows للتحقق من رمي الأخطاء المتوقعة.",code:`# اختبار حالات حدية
دالة قسمة_آمنة(أ، ب)
    إذا (ب == 0)
        ارمي "قسمة على صفر"
    نهاية
    ارجع أ / ب
نهاية

# تأكد من رمي خطأ
حاول
    قسمة_آمنة(10، 0)
    اطبع_سطر("✗ لم يُرم خطأ")
امسك (خطأ)
    اطبع_سطر("✓ القسمة على صفر ترمي خطأ")
نهاية

إذا (قسمة_آمنة(0، 5) == 0)
    اطبع_سطر("✓ صفر مقسوم = 0")
نهاية`,exercise:`# اختبر حالات حدية لدالة القيمة المطلقة
دالة قيمة_مطلقة(ن)
    إذا (ن < 0)
        ارجع -ن
    نهاية
    ارجع ن
نهاية

# أكمل: اختبر الحالات التالية
# 1. رقم موجب
# 2. رقم سالب
# 3. صفر
# 4. رقم كبير جداً`,hint:"تحقق أن قيمة_مطلقة(-5)==5 وقيمة_مطلقة(0)==0.",tips:["اختبر: صفر، سالب، أقصى قيمة","تحقق من رمي الأخطاء المتوقعة","مدخلات فارغة ومدخلات غير صالحة"],concepts:["حالات حدية","اختبار أخطاء","شمولية"]},{id:"l5_66",title:"اختبار التكامل",desc:"اختبار تفاعل الوحدات معاً والتأكد من سلامة النظام الكامل.",explanation:"اختبار التكامل يفحص عمل الوحدات معاً — التوصيلات بينها. أصعب من الوحدوي لأنه يحتاج إعداد بيئة كاملة. سيناريوهات واقعية: سلة مشتريات → حساب سعر → دفع. في CI/CD، تُشغّل عند كل push. التوازن: وحدوي كثير، تكامل معقول، E2E قليل (Testing Pyramid).",code:`# اختبار تكامل — مخزن مع محلل
صنف مخزن
    باني()
        هذا.عناصر = []
    نهاية
    دالة أضف(قيمة)
        هذا.عناصر += [قيمة]
    نهاية
نهاية

دالة مجموع(قيم)
    متغير م = 0
    لكل ق في قيم
        م += ق
    نهاية
    ارجع م
نهاية

متغير مخ = جديد مخزن()
مخ.أضف(10)
مخ.أضف(20)
مخ.أضف(30)
إذا (مجموع(مخ.عناصر) == 60)
    اطبع_سطر("✓ التكامل يعمل")
نهاية`,exercise:`# اختبر تكامل سلة مشتريات مع حاسبة سعر
صنف سلة
    باني()
        هذا.منتجات = []
    نهاية
    دالة أضف(اسم، سعر)
        هذا.منتجات += [{"اسم": اسم، "سعر": سعر}]
    نهاية
نهاية

# أكمل: صنف حاسبة_سعر يحسب الإجمالي
# ثم اكتب اختبار تكامل`,hint:"حاسبة_سعر تأخذ سلة.منتجات وتجمع الأسعار.",tips:["اختبر سيناريوهات كاملة","تحقق من تدفق البيانات بين الوحدات","استخدم بيانات واقعية"],concepts:["اختبار تكامل","تفاعل وحدات","سيناريوهات"]},{id:"l5_67",title:"التطوير بالاختبارات (TDD)",desc:"كتابة الاختبارات أولاً ثم بناء الكود لتحقيقها.",explanation:"TDD (Test-Driven Development) يقلب الترتيب: الاختبار أولاً. الدورة: أحمر (اكتب اختبار فاشل) → أخضر (اكتب أقل كود للنجاح) → إعادة هيكلة (حسّن الكود). الفائدة: تغطية عالية تلقائياً، تصميم أفضل لأنك تفكر بالاستخدام أولاً. يحتاج ممارسة لإتقانه.",code:`# TDD: اكتب الاختبار أولاً
# الخطوة 1: اختبار
صنف مكدس
    باني()
        هذا.عناصر = []
    نهاية
    دالة ادفع(قيمة)
        هذا.عناصر += [قيمة]
    نهاية
    دالة اسحب()
        ارجع هذا.عناصر[طول(هذا.عناصر) - 1]
    نهاية
    دالة الحجم()
        ارجع طول(هذا.عناصر)
    نهاية
نهاية

# الخطوة 2: الاختبار
متغير م = جديد مكدس()
م.ادفع(1)
م.ادفع(2)
إذا (م.اسحب() == 2)
    اطبع_سطر("✓ آخر عنصر يخرج أولاً")
نهاية`,exercise:`# TDD: اكتب اختبارات لـ "طابور" ثم نفّذه
# الاختبار: أول عنصر يدخل أول عنصر يخرج

# الخطوة 1: الاختبار
# تأكد أن إضافة 1 ثم 2 ثم سحب يُرجع 1
# تأكد أن الحجم ينقص بعد السحب

# الخطوة 2: أكمل التنفيذ
صنف طابور
    باني()
        هذا.عناصر = []
    نهاية
    # أكمل: ادفع واسحب والحجم
نهاية`,hint:"اسحب يُرجع عناصر[0] (أول عنصر) بدل الأخير.",tips:["أحمر: اكتب اختبار فاشل","أخضر: اكتب أقل كود للنجاح","إعادة هيكلة: حسّن الكود"],concepts:["TDD","اختبار أولاً","دورة أحمر-أخضر"]},{id:"l5_68",title:"أنماط مراجعة الكود",desc:"تقنيات لكتابة كود نظيف وقابل للمراجعة والصيانة.",explanation:'الكود النظيف يُقرأ كالنثر: أسماء واضحة، دوال قصيرة، مسؤولية واحدة. "Code Tells You How، Comments Tell You Why". تجنب: أسماء مختصرة (x, tmp)، دوال طويلة، تعليقات قديمة. كتاب "Clean Code" مرجع أساسي. المراجعة الثنائية (PR review) تكشف المشاكل مبكراً.',code:`# مثال: كود سيء مقابل كود جيد

# ❌ كود صعب القراءة
دالة ح(م، ن)
    متغير ت = 0
    لكل ع في م
        إذا (ع > ن)
            ت += 1
        نهاية
    نهاية
    ارجع ت
نهاية

# ✓ كود واضح ومسمّى
دالة عدد_أكبر_من(أرقام، حد)
    متغير عدّاد = 0
    لكل رقم في أرقام
        إذا (رقم > حد)
            عدّاد += 1
        نهاية
    نهاية
    ارجع عدّاد
نهاية

# اختبار التكافؤ
متغير بيانات = [5، 10، 15، 20، 25]
إذا (ح(بيانات، 12) == عدد_أكبر_من(بيانات، 12))
    اطبع_سطر("✓ الدالتان متكافئتان")
نهاية
اطبع("النتيجة: " + إلى_نص(عدد_أكبر_من(بيانات، 12)))`,exercise:`# أعد كتابة هذا الكود بأسماء واضحة
دالة ع(ق)
    متغير ن = []
    لكل ت في ق
        متغير م = صحيح
        إذا (ت < 2)
            م = خطأ
        نهاية
        إذا (م)
            ن += [ت]
        نهاية
    نهاية
    ارجع ن
نهاية

# أعد كتابة الدالة بأسماء واضحة
# هذه الدالة تبحث عن الأرقام >= 2`,hint:"أعد التسمية: ع→أرقام_صالحة، ق→قائمة، ن→نتيجة.",tips:["أسماء واضحة أهم من التعليقات","دوال قصيرة بمسؤولية واحدة","تجنب التعقيد غير الضروري"],concepts:["كود نظيف","تسمية واضحة","قابلية قراءة"]},{id:"l5_69",title:"استراتيجيات التصحيح",desc:"تقنيات منهجية لتتبع وإصلاح الأخطاء في البرامج.",explanation:"التصحيح (Debugging) مهارة تتطور بالخبرة. الأساسيات: أعد إنتاج الخطأ، ضيّق المجال (binary search في الكود). التسجيل بمستويات (DEBUG، INFO، ERROR) يسهّل التتبع. في الإنتاج، عطّل DEBUG. أدوات: breakpoints، watch expressions، stack traces. لا تخمّن — اجمع الدليل.",code:`# أداة تصحيح بسيطة
صنف مصحح
    باني(مفعّل)
        هذا.مفعّل = مفعّل
        هذا.سجل = []
    نهاية
    دالة تتبع(رسالة، قيمة)
        إذا (هذا.مفعّل)
            متغير سطر = "[تتبع] " + رسالة + ": " + إلى_نص(قيمة)
            هذا.سجل += [سطر]
            اطبع_سطر(سطر)
        نهاية
    نهاية
نهاية

متغير م = جديد مصحح(صحيح)
م.تتبع("قيمة س"، 42)
م.تتبع("حالة"، "نشط")`,exercise:`# أنشئ مسجّل مع مستويات
صنف مسجّل
    باني(مستوى)
        هذا.مستوى = مستوى
    نهاية
    دالة خطأ(رسالة)
        اطبع_سطر("[خطأ] " + رسالة)
    نهاية
    دالة تحذير(رسالة)
        إذا (هذا.مستوى >= 1)
            اطبع_سطر("[تحذير] " + رسالة)
        نهاية
    نهاية
    # أكمل: دالة معلومة(رسالة) مستوى >= 2
نهاية`,hint:"مثل تحذير لكن تحقق من مستوى >= 2.",tips:["استخدم مستويات للتحكم بالإخراج","سجّل القيم عند نقاط مهمة","عطّل التتبع في الإنتاج"],concepts:["تصحيح","تسجيل","تتبع أخطاء"]},{id:"l5_70",title:"اختبار الأداء",desc:"قياس أداء الكود ومقارنة بدائل التنفيذ.",explanation:`قياس الأداء (Benchmarking) يحوّل التخمين لحقائق. كرّر آلاف المرات لنتائج دقيقة. قارن البدائل بنفس الظروف. احذر من التحسينات المبكرة: قس أولاً، حسّن الأبطأ. أدوات مثل Benchmark.js وhyperfine متخصصة. "Measure، don't guess" قاعدة ذهبية.`,code:`# قياس أداء
صنف مقياس
    باني(اسم)
        هذا.اسم = اسم
        هذا.نتائج = []
    نهاية
    دالة قِس(وصف، دالة_عمل، تكرارات)
        متغير ت = 0
        بينما (ت < تكرارات)
            دالة_عمل()
            ت += 1
        نهاية
        هذا.نتائج += [وصف]
        اطبع_سطر("⏱ " + وصف + ": " + إلى_نص(تكرارات) + " تكرار")
    نهاية
نهاية

متغير م = جديد مقياس("مقارنة")
م.قِس("حلقة"، لامدا()
    متغير مج = 0
    لكل ت في نطاق(100)
        مج += ت
    نهاية
نهاية، 5)`,exercise:`# قِس أداء البحث الخطي
دالة بحث_خطي(مصفوفة، هدف)
    لكل ت في نطاق(طول(مصفوفة))
        إذا (مصفوفة[ت] == هدف)
            ارجع ت
        نهاية
    نهاية
    ارجع -1
نهاية

# أكمل: أنشئ مقياس وقِس البحث في مصفوفات بأحجام مختلفة
# قارن البحث عن أول عنصر مقابل آخر عنصر`,hint:"أنشئ مصفوفات بأحجام 10 و100 وقِس كل حالة.",tips:["كرر عدة مرات للحصول على نتيجة دقيقة","قارن بدائل مختلفة","ركّز على العمليات الأكثر تكراراً"],concepts:["أداء","قياس","مقارنة بدائل"]}]},Vc={id:"l5_complete_projects",title:"مشاريع متكاملة",icon:"🎓",lessons:[{id:"l5_71",title:"قاعدة بيانات مصغرة",desc:"بناء نظام قاعدة بيانات في الذاكرة يدعم CRUD والبحث.",explanation:"قاعدة البيانات في الذاكرة تخزّن الجداول كخرائط والسجلات كمصفوفات — مثل Redis المبسط. هذا المشروع يعلّمك أساسيات CRUD (إنشاء/قراءة/تحديث/حذف) التي هي أساس كل تطبيق. البحث بالمعايير يشبه عبارة WHERE في SQL. في الواقع، قواعد البيانات الاحترافية تضيف فهارس وتخزين دائم ولغة استعلام — لكن الفكرة الأساسية نفسها.",code:`# قاعدة بيانات في الذاكرة
صنف قاعدة_بيانات
    باني()
        هذا.جداول = {}
    نهاية
    دالة أنشئ_جدول(اسم)
        هذا.جداول[اسم] = []
    نهاية
    دالة أدخل(جدول، سجل)
        هذا.جداول[جدول] += [سجل]
    نهاية
    دالة ابحث(جدول، مفتاح، قيمة)
        متغير نتائج = []
        لكل سجل في هذا.جداول[جدول]
            إذا (سجل[مفتاح] == قيمة)
                نتائج += [سجل]
            نهاية
        نهاية
        ارجع نتائج
    نهاية
نهاية

متغير قب = جديد قاعدة_بيانات()
قب.أنشئ_جدول("طلاب")
قب.أدخل("طلاب"، {"اسم": "أحمد"، "عمر": 20})
اطبع(قب.ابحث("طلاب"، "اسم"، "أحمد"))`,exercise:`# أضف دالة حدّث وحذف
صنف مخزن
    باني()
        هذا.بيانات = []
    نهاية
    دالة أضف(عنصر)
        هذا.بيانات += [عنصر]
    نهاية
    # أكمل: دالة ابحث(مفتاح، قيمة)
نهاية`,hint:"كرر على البيانات وقارن المفتاح.",tips:["كل سجل يحمل معرّف فريد","ادعم البحث بعدة معايير","أضف تحقق قبل الإدراج"],concepts:["قاعدة بيانات","CRUD","بحث"]},{id:"l5_72",title:"نظام محادثة",desc:"بناء نظام محادثة مع غرف ورسائل ومستخدمين.",explanation:"أنظمة المحادثة مثل WhatsApp وSlack مبنية على مفاهيم بسيطة: غرف تجمع أعضاء، ورسائل بطابع زمني. نمط Observer يُشعر الأعضاء بالرسائل الجديدة. التحديات الحقيقية: ترتيب الرسائل، الرسائل الخاصة، وإدارة الحالة (متصل/غير متصل). هذا المشروع يدمج OOP مع هياكل البيانات وإدارة الحالة.",code:`# نظام محادثة
صنف غرفة_محادثة
    باني(اسم)
        هذا.اسم = اسم
        هذا.رسائل = []
        هذا.أعضاء = []
    نهاية
    دالة انضم(مستخدم)
        هذا.أعضاء += [مستخدم]
        هذا.رسائل += [{"من": "النظام"، "نص": مستخدم + " انضم"}]
    نهاية
    دالة أرسل(من، نص)
        هذا.رسائل += [{"من": من، "نص": نص}]
    نهاية
نهاية

متغير غ = جديد غرفة_محادثة("عام")
غ.انضم("أحمد")
غ.أرسل("أحمد"، "مرحباً!")
اطبع(غ.رسائل)`,exercise:`# أضف رسائل خاصة
صنف محادثة
    باني()
        هذا.رسائل = []
    نهاية
    دالة أرسل(من، إلى، نص)
        هذا.رسائل += [{"من": من، "إلى": إلى، "نص": نص}]
    نهاية
    # أكمل: دالة رسائل_لـ(مستخدم)
نهاية`,hint:"صفّي الرسائل حيث إلى == مستخدم أو من == مستخدم.",tips:["فصل الرسائل العامة والخاصة","سجّل وقت كل رسالة","حدّد عدد الرسائل المحفوظة"],concepts:["نظام محادثة","غرف","رسائل"]},{id:"l5_73",title:"إدارة المخزون",desc:"نظام متكامل لتتبع المنتجات والمخزون والتنبيهات.",explanation:"إدارة المخزون من أكثر الأنظمة طلباً في الشركات الصغيرة والكبيرة. المبدأ: تتبع كل حركة دخول وخروج لكل منتج. التنبيهات التلقائية عند انخفاض الكمية تمنع نفاد البضاعة. التقارير المالية (قيمة المخزون الإجمالية) تساعد في اتخاذ القرارات. هذا المشروع يجمع بين OOP وإدارة البيانات والمنطق التجاري.",code:`# نظام إدارة مخزون
صنف مخزون
    باني()
        هذا.منتجات = {}
    نهاية
    دالة أضف(اسم، سعر، كمية)
        هذا.منتجات[اسم] = {"سعر": سعر، "كمية": كمية}
    نهاية
    دالة بِع(اسم، كمية)
        إذا (هذا.منتجات[اسم]["كمية"] >= كمية)
            هذا.منتجات[اسم]["كمية"] -= كمية
            ارجع صحيح
        نهاية
        ارجع خطأ
    نهاية
    دالة منخفضة(حد)
        متغير تنبيهات = []
        لكل اسم في هذا.منتجات
            إذا (هذا.منتجات[اسم]["كمية"] < حد)
                تنبيهات += [اسم]
            نهاية
        نهاية
        ارجع تنبيهات
    نهاية
نهاية

متغير م = جديد مخزون()
م.أضف("قلم"، 5، 100)
م.أضف("دفتر"، 15، 3)
م.بِع("قلم"، 10)
اطبع(م.منخفضة(5))`,exercise:`# أضف تقرير مالي
صنف مخزن
    باني()
        هذا.عناصر = []
    نهاية
    دالة أضف(اسم، سعر، كمية)
        هذا.عناصر += [{"اسم": اسم، "سعر": سعر، "كمية": كمية}]
    نهاية
    # أكمل: دالة القيمة() — مجموع سعر*كمية
نهاية`,hint:"القيمة = مجموع سعر*كمية. الأغلى: قارن الأسعار بحلقة.",tips:["تتبع كل حركة دخول وخروج","نبّه عند انخفاض المخزون","احسب القيمة الإجمالية"],concepts:["إدارة مخزون","تتبع","تنبيهات"]},{id:"l5_74",title:"محرك اختبارات (Quiz)",desc:"بناء محرك اختبارات يدعم أنواع أسئلة متعددة وتسجيل النتائج.",explanation:"محركات الاختبارات مثل Kahoot وGoogle Forms أساسها بسيط: أسئلة مع إجابات صحيحة، وآلية تقييم. التوسع يشمل: أنواع أسئلة (اختيارات، صح/خطأ، نصية)، حد زمني، اختيار عشوائي، ومستويات صعوبة. نمط Strategy يتيح إضافة أنواع أسئلة جديدة بدون تعديل المحرك. التصنيف التلقائي يوفر تغذية راجعة فورية.",code:`# محرك اختبارات
صنف اختبار
    باني(عنوان)
        هذا.عنوان = عنوان
        هذا.أسئلة = []
        هذا.درجة = 0
    نهاية
    دالة أضف(نص، إجابة)
        هذا.أسئلة += [{"نص": نص، "إجابة": إجابة}]
    نهاية
    دالة تحقق(رقم، إجابة)
        إذا (هذا.أسئلة[رقم]["إجابة"] == إجابة)
            هذا.درجة += 1
            ارجع صحيح
        نهاية
        ارجع خطأ
    نهاية
نهاية

متغير إ = جديد اختبار("رياضيات")
إ.أضف("2+2؟"، 4)
إ.أضف("3*3؟"، 9)
إ.تحقق(0، 4)
إ.تحقق(1، 9)
اطبع("الدرجة: " + إلى_نص(إ.درجة))`,exercise:`# أضف نظام تصنيف حسب الدرجة
صنف محرك
    باني()
        هذا.أسئلة = []
        هذا.صحيح = 0
    نهاية
    # أكمل: دالة تصنيف() ممتاز/جيد/مقبول
نهاية`,hint:"احسب النسبة (صحيح/عدد الأسئلة) ثم طابق النطاقات.",tips:["ادعم أنواع أسئلة مختلفة","سجّل وقت كل إجابة","وفّر تغذية راجعة فورية"],concepts:["محرك اختبار","تسجيل نتائج","وراثة"]},{id:"l5_75",title:"متتبع المصروفات",desc:"تطبيق لتتبع المصروفات مع تصنيفات وتقارير شهرية.",explanation:"تتبع المصروفات مشروع عملي جداً — كثير من الناس يستخدمون تطبيقات مشابهة يومياً. التصنيف (طعام، نقل، ترفيه) يكشف أنماط الإنفاق. تحديد ميزانية لكل فئة مع تنبيهات عند 80% يساعد على الانضباط المالي. التقارير الشهرية تعطي نظرة شاملة. هذا المشروع يدمج إدارة البيانات مع التحليل والتصور.",code:`# متتبع مصروفات
صنف متتبع
    باني()
        هذا.مصروفات = []
    نهاية
    دالة أضف(وصف، مبلغ، فئة)
        هذا.مصروفات += [{"وصف": وصف، "مبلغ": مبلغ، "فئة": فئة}]
    نهاية
    دالة الإجمالي()
        متغير مج = 0
        لكل م في هذا.مصروفات
            مج += م["مبلغ"]
        نهاية
        ارجع مج
    نهاية
    دالة حسب_فئة()
        متغير فئات = {}
        لكل م في هذا.مصروفات
            إذا (فئات[م["فئة"]] == لاشيء)
                فئات[م["فئة"]] = 0
            نهاية
            فئات[م["فئة"]] += م["مبلغ"]
        نهاية
        ارجع فئات
    نهاية
نهاية

متغير م = جديد متتبع()
م.أضف("بنزين"، 200، "نقل")
م.أضف("غداء"، 50، "طعام")
م.أضف("عشاء"، 70، "طعام")
اطبع(م.حسب_فئة())`,exercise:`# أضف ميزانية مع تنبيهات
صنف ميزانية
    باني(حد)
        هذا.حد = حد
        هذا.منفق = 0
    نهاية
    دالة أنفق(مبلغ)
        هذا.منفق += مبلغ
    نهاية
    # أكمل: دالة تحذير() ترجع صحيح إذا > 80%
نهاية`,hint:"تحذير: منفق > حد * 0.8.",tips:["صنّف المصروفات لتحليل أفضل","نبّه عند قرب تجاوز الحد"],concepts:["تتبع مصروفات","تصنيف","ميزانية"]},{id:"l5_76",title:"إدارة المكتبة",desc:"نظام لإدارة مكتبة كتب مع إعارة وإرجاع وبحث.",explanation:"نظام إدارة المكتبة مثال كلاسيكي على تطبيق CRUD مع حالة: الكتاب إما متاح أو معار. تتبع الإعارة والإرجاع يتطلب إدارة حالة دقيقة — لا تُعِر كتاباً معاراً. البحث بالعنوان والمؤلف والتصنيف يحتاج فهرسة. هذا النمط ينطبق على أي نظام حجوزات (فنادق، سيارات، قاعات).",code:`# إدارة مكتبة كتب
صنف مكتبة
    باني(اسم)
        هذا.اسم = اسم
        هذا.كتب = []
    نهاية
    دالة أضف(عنوان، مؤلف)
        هذا.كتب += [{"عنوان": عنوان، "مؤلف": مؤلف، "معار": خطأ}]
    نهاية
    دالة أعِر(عنوان)
        لكل كتاب في هذا.كتب
            إذا (كتاب["عنوان"] == عنوان و كتاب["معار"] == خطأ)
                كتاب["معار"] = صحيح
                ارجع صحيح
            نهاية
        نهاية
        ارجع خطأ
    نهاية
    دالة المتاحة()
        متغير نتيجة = []
        لكل ك في هذا.كتب
            إذا (ك["معار"] == خطأ)
                نتيجة += [ك["عنوان"]]
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير م = جديد مكتبة("مكتبة المدينة")
م.أضف("البرمجة بلغة ص"، "أحمد")
م.أضف("الخوارزميات"، "سارة")
م.أعِر("البرمجة بلغة ص")
اطبع(م.المتاحة())`,exercise:`# أضف بحث حسب المؤلف
صنف أرشيف
    باني()
        هذا.كتب = []
    نهاية
    دالة أضف(عنوان، مؤلف)
        هذا.كتب += [{"عنوان": عنوان، "مؤلف": مؤلف}]
    نهاية
    # أكمل: دالة بحث_مؤلف(اسم)
نهاية`,hint:'قارن كتاب["مؤلف"] للبحث.',tips:["تتبع حالة كل كتاب","دعم البحث بعدة معايير","سجّل تاريخ الإعارة والإرجاع"],concepts:["إدارة مكتبة","إعارة/إرجاع","بحث"]},{id:"l5_77",title:"تطبيق المهام بالتصنيفات",desc:"تطبيق مهام متقدم مع تصنيفات وأولويات وتصفية.",explanation:"تطبيقات المهام مثل Todoist وTrello من أكثر التطبيقات استخداماً. إضافة التصنيفات والأولويات تحوّل القائمة البسيطة لأداة إنتاجية. التصفية المتعددة (حسب التصنيف، الأولوية، الحالة) تحتاج تصميماً مرناً. الإحصائيات (نسبة الإنجاز، التوزيع) تعطي نظرة شاملة. هذا المشروع يمزج إدارة البيانات مع التصفية والتحليل.",code:`# تطبيق مهام مع تصنيفات
صنف نظام_مهام
    باني()
        هذا.مهام = []
        هذا.رقم = 0
    نهاية
    دالة أضف(عنوان، تصنيف، أولوية)
        هذا.رقم += 1
        هذا.مهام += [{"رقم": هذا.رقم، "عنوان": عنوان، "تصنيف": تصنيف، "أولوية": أولوية، "مكتملة": خطأ}]
    نهاية
    دالة حسب_تصنيف(تصنيف)
        متغير نتيجة = []
        لكل م في هذا.مهام
            إذا (م["تصنيف"] == تصنيف)
                نتيجة += [م]
            نهاية
        نهاية
        ارجع نتيجة
    نهاية
نهاية

متغير ن = جديد نظام_مهام()
ن.أضف("إنهاء التقرير"، "عمل"، "عالية")
ن.أضف("تمارين"، "شخصي"، "متوسطة")
اطبع(ن.حسب_تصنيف("عمل"))`,exercise:`# أضف إحصائيات
صنف مهام
    باني()
        هذا.قائمة = []
    نهاية
    دالة أضف(عنوان، حالة)
        هذا.قائمة += [{"عنوان": عنوان، "حالة": حالة}]
    نهاية
    # أكمل: دالة إحصائيات() تعد كل حالة
نهاية`,hint:"أنشئ خريطة وعدّ كل حالة في حلقة.",tips:["التصنيفات تنظم المهام بصرياً","ادعم تصفية بعدة معايير","أضف بحث في عناوين المهام"],concepts:["تطبيق مهام","تصنيفات","تصفية"]},{id:"l5_78",title:"نظام الدرجات",desc:"نظام لإدارة درجات الطلاب مع حساب المعدلات والتقدير.",explanation:"نظام الدرجات يجمع بين إدارة البيانات والحسابات الرياضية. حساب المعدل التراكمي يتطلب تجميع متعدد المستويات (طالب → مواد → درجات). التقدير (ممتاز/جيد/مقبول) مثال مثالي لمطابقة الأنماط والنطاقات. الترتيب التنافسي بين الطلاب يحتاج خوارزمية ترتيب. هذا النمط ينطبق على أي نظام تقييم.",code:`# نظام درجات
صنف درجات
    باني()
        هذا.طلاب = {}
    نهاية
    دالة سجّل(اسم)
        هذا.طلاب[اسم] = []
    نهاية
    دالة أضف(اسم، مادة، درجة)
        هذا.طلاب[اسم] += [{"مادة": مادة، "درجة": درجة}]
    نهاية
    دالة معدل(اسم)
        متغير مج = 0
        لكل د في هذا.طلاب[اسم]
            مج += د["درجة"]
        نهاية
        ارجع مج / طول(هذا.طلاب[اسم])
    نهاية
    دالة تقدير(اسم)
        متغير م = هذا.معدل(اسم)
        إذا (م >= 90)
            ارجع "ممتاز"
        وإلا
            ارجع "جيد"
        نهاية
    نهاية
نهاية

متغير ن = جديد درجات()
ن.سجّل("أحمد")
ن.أضف("أحمد"، "رياضيات"، 95)
ن.أضف("أحمد"، "علوم"، 88)
اطبع(ن.تقدير("أحمد"))`,exercise:`# أضف ترتيب الطلاب
صنف فصل
    باني()
        هذا.طلاب = {}
    نهاية
    دالة أضف(اسم، درجات)
        هذا.طلاب[اسم] = درجات
    نهاية
    # أكمل: دالة الأعلى() أعلى معدل
نهاية`,hint:"احسب معدل كل طالب ثم قارن للعثور على الأعلى.",tips:["استخدم مطابقة أنماط للتقدير","احسب المعدل التراكمي تلقائياً","ادعم عدة فصول دراسية"],concepts:["نظام درجات","معدلات","مطابقة أنماط"]},{id:"l5_79",title:"مفسر بسيط",desc:"بناء مفسر مصغر يفهم عمليات حسابية ومتغيرات.",explanation:"بناء مفسر هو ذروة فهم البرمجة — لأنك تبني أداة تفهم كوداً وتنفذه. المكونات: تخزين المتغيرات (بيئة)، تحليل العمليات (parsing)، وتنفيذها (evaluation). مطابقة الأنماط تربط كل عامل بعملية. التوسع: دعم الأقواس، الدوال، الشروط. لغة ص نفسها مبنية على هذه المفاهيم — فأنت تتعلم كيف تعمل الأدوات التي تستخدمها.",code:`# مفسر تعبيرات حسابية
صنف مفسر
    باني()
        هذا.متغيرات = {}
    نهاية
    دالة عيّن(اسم، قيمة)
        هذا.متغيرات[اسم] = قيمة
    نهاية
    دالة نفّذ(أ، عملية، ب)
        متغير ق1 = هذا.متغيرات[أ]
        إذا (ق1 == لاشيء)
            ق1 = إلى_رقم(أ)
        نهاية
        متغير ق2 = هذا.متغيرات[ب]
        إذا (ق2 == لاشيء)
            ق2 = إلى_رقم(ب)
        نهاية
        طابق (عملية)
            عندما "+":
                ارجع ق1 + ق2
            عندما "*":
                ارجع ق1 * ق2
        نهاية
    نهاية
نهاية

متغير م = جديد مفسر()
م.عيّن("س"، 10)
اطبع(م.نفّذ("س"، "+"، "5"))`,exercise:`# أضف دوال للمفسر
صنف مفسر_دوال
    باني()
        هذا.دوال = {}
    نهاية
    دالة عرّف(اسم، جسم)
        هذا.دوال[اسم] = جسم
    نهاية
    # أكمل: دالة استدعِ(اسم، وسيط)
نهاية`,hint:"خزّن الجسم كلامدا واستدعِه بالوسائط المعطاة.",tips:["فصل التحليل عن التنفيذ","خزّن المتغيرات في بيئة","دعم التعابير المتداخلة"],concepts:["مفسر","تقييم تعابير","متغيرات"]},{id:"l5_80",title:"المشروع الختامي",desc:"مشروع شامل يجمع جميع المفاهيم: نظام إدارة متكامل.",explanation:"المشروع الختامي يجمع كل ما تعلمته في 300 درس: OOP لتنظيم الكود، هياكل البيانات لتخزين المعلومات، الخوارزميات للبحث والترتيب، إدارة الأخطاء للمتانة، وأنماط التصميم للمرونة. المشروع الحقيقي يختلف عن التمارين — يحتاج تخطيطاً وتصميماً وتفكيراً في التوسع. اعتبر هذا تخرّجك كمبرمج: الآن تستطيع بناء أي نظام تتخيله!",code:`# نظام إدارة متكامل
صنف نظام_إدارة
    باني(اسم)
        هذا.اسم = اسم
        هذا.مستخدمون = {}
        هذا.بيانات = {}
        هذا.سجل = []
    نهاية
    دالة أضف_مستخدم(اسم، دور)
        هذا.مستخدمون[اسم] = دور
        هذا.سجل += ["إضافة: " + اسم]
    نهاية
    دالة أضف_بيانات(فئة، عنصر)
        إذا (هذا.بيانات[فئة] == لاشيء)
            هذا.بيانات[فئة] = []
        نهاية
        هذا.بيانات[فئة] += [عنصر]
    نهاية
    دالة تقرير()
        اطبع_سطر("النظام: " + هذا.اسم)
        اطبع_سطر("أحداث: " + إلى_نص(طول(هذا.سجل)))
    نهاية
نهاية

متغير ن = جديد نظام_إدارة("المدرسة")
ن.أضف_مستخدم("أحمد"، "معلم")
ن.أضف_بيانات("مواد"، "رياضيات")
ن.تقرير()`,exercise:`# وسّع النظام بصلاحيات
صنف نظام
    باني()
        هذا.مستخدمون = {}
        هذا.صلاحيات = {}
    نهاية
    دالة أضف(اسم، دور)
        هذا.مستخدمون[اسم] = دور
    نهاية
    # أكمل: دالة يملك_صلاحية(اسم، صلاحية)
نهاية`,hint:"احصل على دور المستخدم ثم ابحث في صلاحيات[دور].",tips:["اجمع كل المفاهيم في مشروع واحد","صمم بشكل وحداتي قابل للتوسيع"],concepts:["مشروع متكامل","صلاحيات","تصميم نظام"]}]},Mc={id:5,title:"المحترف",titleEn:"Professional",description:"البرمجة الوظيفية المتقدمة، هياكل البيانات، الخوارزميات، ومشاريع متكاملة.",icon:"🏆",color:"#ef4444",sections:[Pc,Tc,Oc,Ic,wc,Ec,qc,Vc]},Ac=[tc,cc,mc,Lc,Mc],Pt="sad-learn-progress";function Dc(){try{const e=localStorage.getItem(Pt);if(e)return JSON.parse(e)}catch{}return{completed:{},lastLesson:null,lastLevel:1}}function Bc(e){try{localStorage.setItem(Pt,JSON.stringify(e))}catch{}}const oe=C(Dc());ne(oe,e=>Bc(e),{deep:!0});function Nc(){const e=O(()=>Object.keys(oe.value.completed).length);function n(h){return!!oe.value.completed[h]}function s(h){oe.value.completed[h]=!0}function a(h){delete oe.value.completed[h],oe.value={...oe.value}}function o(h){n(h)?a(h):s(h)}function c(h,S){if(S.length===0)return 0;const f=S.filter(b=>n(b)).length;return Math.round(f/S.length*100)}function r(h){if(h.length===0)return 0;const S=h.filter(f=>n(f)).length;return Math.round(S/h.length*100)}function d(h,S){oe.value.lastLesson=h,oe.value.lastLevel=S}function _(){return oe.value.lastLevel||1}function p(){return oe.value.lastLesson}function m(){oe.value={completed:{},lastLesson:null,lastLevel:1}}return{completedCount:e,isCompleted:n,markCompleted:s,markIncomplete:a,toggleCompleted:o,getLevelProgress:c,getSectionProgress:r,setLastLesson:d,getLastLevel:_,getLastLesson:p,resetProgress:m}}const Fc={class:"level-tabs"},Rc=["onClick"],Hc={class:"tab-icon"},zc={class:"tab-info"},Uc={class:"tab-title"},Gc={class:"tab-subtitle"},jc={class:"tab-count"},Jc={viewBox:"0 0 36 36",class:"progress-ring"},Qc={key:0,class:"ring-star"},Wc={key:1,class:"ring-text"},Kc=w({__name:"LevelTabs",props:{levels:{},activeLevel:{},getProgress:{type:Function}},emits:["select"],setup(e){const n=e;function s(c){const r=c.id===n.activeLevel;return{"--tab-color":c.color,borderColor:r?c.color:"transparent",background:r?`${c.color}18`:void 0}}function a(c){return c.sections.reduce((r,d)=>r+d.lessons.length,0)}function o(c){return`${n.getProgress(c)}, 100`}return(c,r)=>(i(),l("div",Fc,[(i(!0),l(F,null,j(e.levels,(d,_)=>(i(),l("button",{key:d.id,class:N(["level-tab",{active:d.id===e.activeLevel,completed:e.getProgress(d.id)>=100}]),style:W({...s(d),animationDelay:`${_*80}ms`}),onClick:p=>c.$emit("select",d.id)},[t("span",Hc,v(d.icon),1),t("div",zc,[t("span",Uc,v(d.title),1),t("span",Gc,v(d.titleEn),1),t("span",jc,v(a(d))+" درس",1)]),t("div",{class:N(["tab-progress-ring",{"ring-pulse":d.id===e.activeLevel}])},[(i(),l("svg",Jc,[r[0]||(r[0]=t("path",{class:"ring-bg",d:"M18 2.0845a15.9155 15.9155 0 010 31.831 15.9155 15.9155 0 010-31.831"},null,-1)),t("path",{class:"ring-fill",style:W({stroke:d.color,strokeDasharray:o(d.id)}),d:"M18 2.0845a15.9155 15.9155 0 010 31.831 15.9155 15.9155 0 010-31.831"},null,4)])),e.getProgress(d.id)>=100?(i(),l("span",Qc,"⭐")):(i(),l("span",Wc,v(e.getProgress(d.id))+"%",1))],2),r[1]||(r[1]=t("div",{class:"tab-indicator"},null,-1))],14,Rc))),128))]))}}),Tt=q(Kc,[["__scopeId","data-v-c3ac0b6c"]]),Yc={class:"section-list"},Xc={key:0,class:"empty-state"},Zc=["onClick"],er={class:"section-icon"},tr={class:"section-info"},nr={class:"section-title"},sr={class:"section-meta"},or={class:"section-progress-bar"},ir={class:"section-pct"},ar={key:0,class:"done-badge"},lr={key:0,class:"lesson-grid"},cr=["onClick"],rr={key:0,class:"check-anim"},dr={class:"lesson-info"},_r={class:"lesson-title"},ur={class:"lesson-desc"},pr={class:"lesson-tags"},vr=["onClick"],hr={class:"quiz-label"},fr=w({__name:"SectionList",props:{sections:{},levelColor:{},isCompleted:{type:Function},getSectionProgress:{type:Function},hasQuiz:{type:Function}},emits:["openLesson","startQuiz"],setup(e){const n=e,s=Kt({});function a(d){s[d]=!s[d]}function o(d){return d.lessons.length>0&&d.lessons.every(_=>n.isCompleted(_.id))}function c(d,_){const p=d;p.style.overflow="hidden",p.style.height="0",p.offsetHeight,p.style.transition="height .35s ease, opacity .3s ease",p.style.height=p.scrollHeight+"px",p.style.opacity="1",p.addEventListener("transitionend",function m(){p.removeEventListener("transitionend",m),p.style.height="",p.style.overflow="",_()})}function r(d,_){const p=d;p.style.overflow="hidden",p.style.height=p.scrollHeight+"px",p.offsetHeight,p.style.transition="height .3s ease, opacity .2s ease",p.style.height="0",p.style.opacity="0",p.addEventListener("transitionend",function m(){p.removeEventListener("transitionend",m),_()})}return(d,_)=>(i(),l("div",Yc,[e.sections.length?g("",!0):(i(),l("div",Xc,[..._[0]||(_[0]=[t("span",{class:"empty-icon"},"📚",-1),t("p",{class:"empty-text"},"لا توجد أقسام بعد — ترقب المحتوى الجديد!",-1)])])),(i(!0),l(F,null,j(e.sections,p=>(i(),l("div",{key:p.id,class:N(["section-card",{"section-done":o(p)}])},[t("div",{class:"accent-strip",style:W({background:o(p)?"#22c55e":e.levelColor})},null,4),t("button",{class:N(["section-header",{open:s[p.id]}]),onClick:m=>a(p.id)},[t("span",er,v(p.icon),1),t("div",tr,[t("span",nr,v(p.title),1),t("span",sr,v(p.lessons.length)+" درس",1)]),t("div",or,[t("div",{class:"sp-fill",style:W({width:e.getSectionProgress(p.lessons.map(m=>m.id))+"%",background:o(p)?"#22c55e":`linear-gradient(90deg, ${e.levelColor}, ${e.levelColor}dd)`})},null,4)]),t("span",ir,v(e.getSectionProgress(p.lessons.map(m=>m.id)))+"%",1),o(p)?(i(),l("span",ar,"✓")):g("",!0),t("span",{class:N(["chevron",{rotated:s[p.id]}])},"▼",2)],10,Zc),E(se,{onEnter:c,onLeave:r},{default:y(()=>[s[p.id]?(i(),l("div",lr,[(i(!0),l(F,null,j(p.lessons,(m,h)=>(i(),l("button",{key:m.id,class:N(["lesson-card",{completed:e.isCompleted(m.id)}]),style:W({"--stagger":h}),onClick:S=>d.$emit("openLesson",m,p.id)},[t("span",{class:"lesson-num",style:W({background:e.isCompleted(m.id)?e.levelColor:void 0})},[e.isCompleted(m.id)?(i(),l("span",rr,"✓")):(i(),l(F,{key:1},[Q(v(h+1),1)],64))],4),t("div",dr,[t("span",_r,v(m.title),1),t("span",ur,v(m.desc),1)]),t("div",pr,[(i(!0),l(F,null,j(m.concepts.slice(0,3),S=>(i(),l("span",{key:S,class:"tag"},v(S),1))),128))])],14,cr))),128)),e.hasQuiz&&e.hasQuiz(p.id)?(i(),l("button",{key:0,class:"quiz-btn",style:W({"--accent":e.levelColor}),onClick:Te(m=>d.$emit("startQuiz",p.id,p.title),["stop"])},[_[1]||(_[1]=t("span",{class:"quiz-icon"},"📝",-1)),t("span",hr,"اختبار: "+v(p.title),1),_[2]||(_[2]=t("span",{class:"quiz-arrow"},"←",-1))],12,vr)):g("",!0)])):g("",!0)]),_:2},1024)],2))),128))]))}}),Ot=q(fr,[["__scopeId","data-v-27dd7e9d"]]),mr=C(null),Ce=C(!1),Le=C(!1),Ke=C(null);let Me=null;async function gr(){if(Le.value)return!0;if(Ce.value)return new Promise(e=>{const n=setInterval(()=>{Ce.value||(clearInterval(n),e(Le.value))},100)});Ce.value=!0,Ke.value=null;try{const e=document.createElement("script");e.src="/sad.js",await new Promise((o,c)=>{e.onload=()=>o(),e.onerror=()=>c(new Error("فشل تحميل ملف sad.js")),document.head.appendChild(e)});const n=window.SadWasm;if(!n)throw new Error("SadWasm factory not found");const s=await n({locateFile:o=>o.endsWith(".wasm")?"/sad.wasm":o});mr.value=s;const a=s.cwrap("sad_execute","number",["string","string"]);return Me=(o,c="")=>{const r=a(o,c),d=s.UTF8ToString(r);return s._free(r),d},Le.value=!0,window.__sadWasm={runCode:(o,c)=>{try{const r=Me(o,c||""),d=JSON.parse(r);return{output:d.output||"",error:d.error||""}}catch(r){return{output:"",error:`خطأ WASM: ${r==null?void 0:r.message}`}}}},!0}catch(e){return Ke.value=(e==null?void 0:e.message)||"فشل تحميل WASM",Le.value=!1,!1}finally{Ce.value=!1}}function It(){return{loading:Ee(Ce),loaded:Ee(Le),error:Ee(Ke),loadWasm:gr,runCode(e,n=""){if(!Me)return{output:"",error:"WASM غير محمّل بعد"};try{const s=Me(e,n),a=JSON.parse(s);return{output:a.output||"",error:a.error||""}}catch(s){return{output:"",error:`خطأ: ${s==null?void 0:s.message}`}}}}}const xr={class:"code-runner"},kr={class:"runner-header"},br={class:"header-label"},yr={class:"header-actions"},$r={key:0,class:"copied-badge"},Sr={class:"editor-wrapper"},Cr={class:"status-bar"},Lr={class:"runner-toolbar"},Pr=["disabled"],Tr={key:0,class:"spinner",width:"16",height:"16",viewBox:"0 0 24 24",fill:"none",stroke:"currentColor","stroke-width":"2.5"},Or={key:0,class:"wasm-status loading"},Ir={key:1,class:"wasm-status ready"},wr={class:"output-header"},Er={class:"output-tabs"},qr={class:"output-actions"},Vr={key:0,class:"copied-badge"},Mr={key:0,class:"output-text"},Ar={key:1,class:"output-text error-text"},Dr=w({__name:"SadCodeRunner",props:{initialCode:{},filename:{}},setup(e){const{loaded:n,loading:s,loadWasm:a}=It(),o=e;ee(()=>{a()});const c=C(),r=C(!1),d=C(""),_=C(""),p=C("output"),m=C(!1),h=C(!1),S=C(!1);let f=null;const b=O(()=>{var k,$;return(($=(k=f==null?void 0:f.state)==null?void 0:k.doc)==null?void 0:$.lines)??o.initialCode.split(`
`).length}),I=O(()=>{var k,$;return(($=(k=f==null?void 0:f.state)==null?void 0:k.doc)==null?void 0:$.length)??o.initialCode.length});async function M(){var k;if(c.value){(k=f==null?void 0:f.destroy)==null||k.call(f),c.value.innerHTML="";try{const[{EditorState:$},{EditorView:D,keymap:K,lineNumbers:G,highlightActiveLine:J},{defaultKeymap:z,history:ie,historyKeymap:pe},{indentOnInput:ve,syntaxHighlighting:he,defaultHighlightStyle:fe},{closeBrackets:ae,closeBracketsKeymap:be},{oneDark:ye}]=await Promise.all([Z(()=>import("./index.tqIctuLa.js"),[]),Z(()=>import("./index.BPH623Bm.js").then(me=>me.i),__vite__mapDeps([2,3])),Z(()=>import("./index.CyGSWCqK.js"),__vite__mapDeps([4,3,2,5])),Z(()=>import("./index.DGTKRUWM.js").then(me=>me.d),__vite__mapDeps([5,3,2])),Z(()=>import("./index.pqLJMZUs.js"),__vite__mapDeps([6,3,2,5])),Z(()=>import("./index.D9sbPGjv.js"),__vite__mapDeps([7,2,3,5]))]);f=new D({state:$.create({doc:o.initialCode,extensions:[G(),J(),ie(),ve(),he(fe,{fallback:!0}),ae(),K.of([...be,...z,...pe]),ye,D.theme({"&":{height:"200px",fontSize:"13px"},".cm-content":{direction:"ltr",textAlign:"left"},".cm-scroller":{overflow:"auto",fontFamily:"JetBrains Mono, monospace"}}),K.of([{key:"Ctrl-Enter",run:()=>(H(),!0)}])]}),parent:c.value})}catch{const $=document.createElement("textarea");$.value=o.initialCode,$.style.cssText="width:100%;height:200px;background:#0d1117;color:#e6edf3;padding:10px;font-family:monospace;font-size:13px;border:none;resize:none;direction:ltr;border-radius:8px;",c.value.appendChild($),f={state:{doc:{toString:()=>$.value,length:$.value.length,lines:$.value.split(`
`).length}},dispatch({changes:D}){(D==null?void 0:D.insert)!==void 0&&($.value=D.insert)},destroy(){}},$.addEventListener("input",()=>{f.state.doc={toString:()=>$.value,length:$.value.length,lines:$.value.split(`
`).length}})}}}async function H(){if(!f)return;const k=f.state.doc.toString();r.value=!0,d.value="",_.value="",S.value=!1;try{const $=window.__sadWasm;if($!=null&&$.runCode){const D=$.runCode(k);D.error?_.value=D.error:d.value=D.output}else{const K=await(await fetch("/api/run",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({code:k})})).json();K.error?_.value=K.error:d.value=K.output??""}}catch($){_.value=`خطأ: ${$==null?void 0:$.message}`}finally{r.value=!1,p.value=_.value?"error":"output",!_.value&&d.value&&(await Pe(),S.value=!0,setTimeout(()=>{S.value=!1},600))}}async function V(){var $,D;const k=((D=($=f==null?void 0:f.state)==null?void 0:$.doc)==null?void 0:D.toString())??o.initialCode;await navigator.clipboard.writeText(k),m.value=!0,setTimeout(()=>{m.value=!1},1500)}async function P(){const k=p.value==="error"?_.value:d.value;await navigator.clipboard.writeText(k),h.value=!0,setTimeout(()=>{h.value=!1},1500)}function L(){d.value="",_.value=""}function T(){d.value="",_.value="",f==null||f.dispatch({changes:{from:0,to:f.state.doc.length,insert:o.initialCode}})}return ne(()=>o.initialCode,()=>{T()}),ee(M),de(()=>{var k;return(k=f==null?void 0:f.destroy)==null?void 0:k.call(f)}),(k,$)=>(i(),l("div",xr,[t("div",kr,[t("span",br,[$[2]||($[2]=t("svg",{width:"14",height:"14",viewBox:"0 0 24 24",fill:"none",stroke:"currentColor","stroke-width":"2"},[t("path",{d:"M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z"}),t("polyline",{points:"13 2 13 9 20 9"})],-1)),Q(" "+v(e.filename||"محرر ص"),1)]),t("div",yr,[t("button",{class:"icon-btn",title:"نسخ الكود",onClick:V},[$[3]||($[3]=t("svg",{width:"14",height:"14",viewBox:"0 0 24 24",fill:"none",stroke:"currentColor","stroke-width":"2"},[t("rect",{x:"9",y:"9",width:"13",height:"13",rx:"2"}),t("path",{d:"M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"})],-1)),m.value?(i(),l("span",$r,"✓")):g("",!0)])])]),t("div",Sr,[t("div",{ref_key:"editorEl",ref:c,class:"editor-mount"},null,512)]),t("div",Cr,[t("span",null,v(b.value)+" سطر · "+v(I.value)+" حرف",1),$[4]||($[4]=t("span",{class:"status-hint"},"Ctrl+Enter للتشغيل",-1))]),t("div",Lr,[t("button",{class:"run-btn",disabled:r.value,title:"تشغيل (Ctrl+Enter)",onClick:H},[r.value?(i(),l("svg",Tr,[...$[5]||($[5]=[t("circle",{cx:"12",cy:"12",r:"10","stroke-dasharray":"31.4 31.4","stroke-linecap":"round"},null,-1)])])):(i(),l(F,{key:1},[Q("▶")],64)),Q(" "+v(r.value?"جارٍ التشغيل...":"تشغيل"),1)],8,Pr),t("button",{class:"reset-btn",onClick:T},"↺ إعادة"),u(s)?(i(),l("span",Or,"⏳ تحميل المحرك...")):u(n)?(i(),l("span",Ir,"● جاهز")):g("",!0)]),E(se,{name:"output-fade"},{default:y(()=>[d.value||_.value?(i(),l("div",{key:0,class:N(["output-panel",{"has-error":!!_.value,"success-flash":S.value}])},[t("div",wr,[t("div",Er,[d.value?(i(),l("button",{key:0,class:N(["tab-btn",{active:p.value==="output"}]),onClick:$[0]||($[0]=D=>p.value="output")},"📤 المخرجات",2)):g("",!0),_.value?(i(),l("button",{key:1,class:N(["tab-btn tab-error",{active:p.value==="error"}]),onClick:$[1]||($[1]=D=>p.value="error")},[...$[6]||($[6]=[t("svg",{width:"12",height:"12",viewBox:"0 0 24 24",fill:"none",stroke:"currentColor","stroke-width":"2.5"},[t("circle",{cx:"12",cy:"12",r:"10"}),t("line",{x1:"15",y1:"9",x2:"9",y2:"15"}),t("line",{x1:"9",y1:"9",x2:"15",y2:"15"})],-1),Q(" خطأ ",-1)])],2)):g("",!0)]),t("div",qr,[t("button",{class:"icon-btn",title:"نسخ المخرجات",onClick:P},[$[7]||($[7]=t("svg",{width:"13",height:"13",viewBox:"0 0 24 24",fill:"none",stroke:"currentColor","stroke-width":"2"},[t("rect",{x:"9",y:"9",width:"13",height:"13",rx:"2"}),t("path",{d:"M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"})],-1)),h.value?(i(),l("span",Vr,"✓")):g("",!0)]),t("button",{class:"icon-btn",title:"مسح",onClick:L},"✕")])]),p.value==="output"&&d.value?(i(),l("pre",Mr,v(d.value),1)):g("",!0),p.value==="error"&&_.value?(i(),l("pre",Ar,v(_.value),1)):g("",!0)],2)):g("",!0)]),_:1})]))}}),Ye=q(Dr,[["__scopeId","data-v-6249e583"]]),wt="sad-learn-notes";function Br(){if(typeof window>"u")return{notes:{},lastModified:0};try{const e=localStorage.getItem(wt);if(e){const n=JSON.parse(e);return{notes:n.notes||{},lastModified:n.lastModified||0}}}catch{}return{notes:{},lastModified:0}}function Nr(e){if(!(typeof window>"u"))try{localStorage.setItem(wt,JSON.stringify(e))}catch{}}const X=C(Br());ne(X,e=>Nr(e),{deep:!0});function lt(){function e(b){return X.value.notes[b]||null}function n(b,I,M){const H=Date.now(),V=X.value.notes[b];X.value.notes[b]={lessonId:b,content:I,createdAt:(V==null?void 0:V.createdAt)||H,updatedAt:H,bookmarked:(V==null?void 0:V.bookmarked)||!1,lessonTitle:(M==null?void 0:M.lessonTitle)||(V==null?void 0:V.lessonTitle),sectionId:(M==null?void 0:M.sectionId)||(V==null?void 0:V.sectionId),levelId:(M==null?void 0:M.levelId)??(V==null?void 0:V.levelId)},X.value.lastModified=H}function s(b){X.value.notes[b]&&(delete X.value.notes[b],X.value.lastModified=Date.now())}function a(b,I){var V;const M=X.value.notes[b],H=Date.now();return M?(M.bookmarked=!M.bookmarked,M.updatedAt=H):X.value.notes[b]={lessonId:b,content:"",createdAt:H,updatedAt:H,bookmarked:!0,lessonTitle:I==null?void 0:I.lessonTitle,sectionId:I==null?void 0:I.sectionId,levelId:I==null?void 0:I.levelId},X.value.lastModified=H,((V=X.value.notes[b])==null?void 0:V.bookmarked)??!1}function o(b){var I;return((I=X.value.notes[b])==null?void 0:I.bookmarked)??!1}function c(b){const I=X.value.notes[b];return!!I&&I.content.trim().length>0}function r(){return Object.values(X.value.notes).filter(b=>b.content.trim().length>0||b.bookmarked).sort((b,I)=>I.updatedAt-b.updatedAt)}function d(){return Object.values(X.value.notes).filter(b=>b.bookmarked).sort((b,I)=>I.updatedAt-b.updatedAt)}function _(b){if(!b.trim())return r();const I=b.trim().toLowerCase();return r().filter(M=>M.content.toLowerCase().includes(I)||M.lessonTitle&&M.lessonTitle.toLowerCase().includes(I))}function p(b){return r().filter(I=>I.levelId===b)}function m(){return Object.values(X.value.notes).filter(b=>b.content.trim().length>0).length}function h(){return Object.values(X.value.notes).filter(b=>b.bookmarked).length}function S(){const b=r();if(b.length===0)return"لا توجد ملاحظات";let I=`# ملاحظاتي — منصة تعلم لغة ص
`;I+=`# التاريخ: ${new Date().toLocaleDateString("ar-SA")}
`,I+=`# عدد الملاحظات: ${b.length}

`;for(const M of b)I+=`## ${M.lessonTitle||M.lessonId}
`,M.bookmarked&&(I+=`⭐ إشارة مرجعية
`),I+=`${M.content}
`,I+=`— آخر تعديل: ${new Date(M.updatedAt).toLocaleDateString("ar-SA")}

`,I+=`---

`;return I}function f(){X.value.notes={},X.value.lastModified=Date.now()}return{getNote:e,hasNote:c,isBookmarked:o,getAllNotes:r,getBookmarkedNotes:d,searchNotes:_,getNotesByLevel:p,getNotesCount:m,getBookmarksCount:h,saveNote:n,deleteNote:s,toggleBookmark:a,clearAllNotes:f,exportNotes:S}}const Fr={class:"toggle-icon"},Rr={key:0,class:"has-note-dot"},Hr={class:"toggle-actions"},zr=["title"],Ur={key:0,class:"notes-body"},Gr={class:"notes-editor"},jr={class:"notes-footer"},Jr={class:"char-count"},Qr={key:0,class:"save-status"},Wr={class:"notes-actions"},Kr=w({__name:"NotesPanel",props:{lessonId:{},lessonTitle:{},sectionId:{},levelId:{},levelColor:{}},setup(e){const n=e,{getNote:s,saveNote:a,deleteNote:o,toggleBookmark:c,isBookmarked:r}=lt(),d=C(!1),_=C(""),p=C(!1),m=C(null),h=C(!1),S=C(!1),f=C();let b=null;function I(){const k=s(n.lessonId);k?(_.value=k.content,m.value=k.updatedAt,S.value=k.content.trim().length>0):(_.value="",m.value=null,S.value=!1),p.value=r(n.lessonId)}function M(){b&&clearTimeout(b),b=setTimeout(()=>{_.value.trim()?(a(n.lessonId,_.value,{lessonTitle:n.lessonTitle,sectionId:n.sectionId,levelId:n.levelId}),m.value=Date.now(),S.value=!0):S.value&&(o(n.lessonId),m.value=null,S.value=!1)},600)}function H(){d.value=!d.value,d.value&&Pe(()=>{var k;return(k=f.value)==null?void 0:k.focus()})}function V(){const k=c(n.lessonId,{lessonTitle:n.lessonTitle,sectionId:n.sectionId,levelId:n.levelId});p.value=k}function P(){confirm("هل تريد حذف هذه الملاحظة؟")&&(o(n.lessonId),_.value="",m.value=null,S.value=!1)}function L(){navigator.clipboard.writeText(_.value),h.value=!0,setTimeout(()=>{h.value=!1},1500)}function T(k){const $=Math.floor((Date.now()-k)/1e3);return $<10?"الآن":$<60?`منذ ${$} ث`:$<3600?`منذ ${Math.floor($/60)} د`:$<86400?`منذ ${Math.floor($/3600)} س`:`منذ ${Math.floor($/86400)} ي`}return ne(()=>n.lessonId,()=>{I(),d.value=!1}),ee(()=>I()),(k,$)=>(i(),l("div",{class:N(["notes-panel",{expanded:d.value}])},[t("button",{class:"notes-toggle",onClick:H,style:W({"--accent":e.levelColor})},[t("span",Fr,v(d.value?"▼":"◀"),1),$[1]||($[1]=t("span",{class:"toggle-icon-emoji"},"📝",-1)),$[2]||($[2]=t("span",{class:"toggle-title"},"ملاحظاتي",-1)),S.value&&!d.value?(i(),l("span",Rr)):g("",!0),t("span",Hr,[t("button",{class:N(["bookmark-btn",{active:p.value}]),onClick:Te(V,["stop"]),title:p.value?"إزالة الإشارة المرجعية":"إضافة إشارة مرجعية"},v(p.value?"⭐":"☆"),11,zr)])],4),E(se,{name:"notes-slide"},{default:y(()=>[d.value?(i(),l("div",Ur,[t("div",Gr,[Re(t("textarea",{ref_key:"textareaRef",ref:f,"onUpdate:modelValue":$[0]||($[0]=D=>_.value=D),class:"notes-textarea",placeholder:`اكتب ملاحظاتك هنا...

💡 الملاحظات تُحفظ تلقائياً`,rows:"5",dir:"rtl",onInput:M},null,544),[[et,_.value]]),t("div",jr,[t("span",Jr,v(_.value.length)+" حرف",1),m.value?(i(),l("span",Qr,[$[3]||($[3]=t("span",{class:"save-dot"},null,-1)),Q(" حُفظ "+v(T(m.value)),1)])):g("",!0),t("div",Wr,[_.value.trim()?(i(),l("button",{key:0,class:"action-btn delete-btn",onClick:P,title:"حذف الملاحظة"}," 🗑️ ")):g("",!0),_.value.trim()?(i(),l("button",{key:1,class:"action-btn copy-btn",onClick:L,title:"نسخ الملاحظة"},v(h.value?"✓":"📋"),1)):g("",!0)])])])])):g("",!0)]),_:1})],2))}}),Yr=q(Kr,[["__scopeId","data-v-46cb462e"]]),Xr={class:"wizard-topbar"},Zr={class:"topbar-center"},ed={class:"lesson-title"},td={class:"progress-dots"},nd={class:"step-counter"},sd={class:"step-indicator"},od={class:"step-track"},id={class:"track-line"},ad=["onClick"],ld={class:"circle-inner"},cd={class:"circle-label"},rd={class:"desc"},dd={key:0,class:"explanation-box"},_d=["innerHTML"],ud={key:1,class:"tips-box"},pd={key:2,class:"concepts-row"},vd={class:"exercise-text"},hd={key:0,class:"hint-box"},fd={key:0,class:"celebration"},md={class:"wizard-nav"},gd=w({__name:"LessonWizard",props:{lesson:{},lessonIndex:{},totalLessons:{},isCompleted:{type:Boolean},levelColor:{},sectionId:{},levelId:{}},emits:["close","complete","next","prev"],setup(e,{emit:n}){const s=e,a=n,o=C(0),c=C(!1),r=C("slide-left"),d=C(!1),_=C(),p=C(),m=[{icon:"📖",label:"الشرح"},{icon:"💻",label:"الكود"},{icon:"🎯",label:"التمرين"}],h=O(()=>`# ${s.lesson.exercise}

`);function S(V){return V.split(`

`).map(P=>{const L=P.trim();return L?L.startsWith("- ")||L.startsWith("• ")?`<ul>${L.split(`
`).map($=>`<li>${$.replace(/^[-•]\s*/,"")}</li>`).join("")}</ul>`:`<p>${L.replace(/`([^`]+)`/g,"<code>$1</code>")}</p>`:""}).join("")}function f(){Pe(()=>{var V;return(V=p.value)==null?void 0:V.scrollTo({top:0,behavior:"smooth"})})}function b(V){r.value=V>o.value?"slide-left":"slide-right",o.value=V,f()}function I(){r.value="slide-left",o.value++,f()}function M(){r.value="slide-right",o.value--,f()}function H(){d.value=!0,a("complete"),setTimeout(()=>{d.value=!1},2200)}return ne(()=>s.lesson.id,()=>{o.value=0,c.value=!1,d.value=!1}),(V,P)=>(i(),l("div",{class:"lesson-wizard",ref_key:"wizardEl",ref:_},[t("div",Xr,[t("button",{class:"back-btn",onClick:P[0]||(P[0]=L=>V.$emit("close")),"aria-label":"رجوع"},[...P[4]||(P[4]=[t("span",{class:"back-icon"},"✕",-1),t("span",{class:"back-text"},"رجوع",-1)])]),t("div",Zr,[t("span",ed,v(e.lesson.title),1),t("div",td,[(i(!0),l(F,null,j(e.totalLessons,L=>(i(),l("span",{key:L,class:N(["pdot",{current:L-1===e.lessonIndex,done:L-1<e.lessonIndex}]),style:W({"--clr":e.levelColor})},null,6))),128))])]),t("span",nd,v(e.lessonIndex+1)+"/"+v(e.totalLessons),1)]),t("div",sd,[t("div",od,[t("div",id,[t("div",{class:"track-fill",style:W({width:o.value/2*100+"%",background:e.levelColor})},null,4)]),(i(),l(F,null,j(m,(L,T)=>t("button",{key:T,class:N(["step-circle",{active:o.value===T,done:o.value>T}]),style:W({"--clr":e.levelColor}),onClick:k=>b(T)},[t("span",ld,[o.value>T?(i(),l(F,{key:0},[Q("✓")],64)):(i(),l(F,{key:1},[Q(v(T+1),1)],64))]),t("span",cd,v(L.label),1)],14,ad)),64))])]),t("div",{class:"step-content",ref_key:"contentEl",ref:p},[E(se,{name:r.value,mode:"out-in"},{default:y(()=>{var L,T;return[(i(),l("div",{key:o.value,class:"step-card"},[o.value===0?(i(),l(F,{key:0},[t("p",rd,v(e.lesson.desc),1),e.lesson.explanation?(i(),l("div",dd,[P[5]||(P[5]=t("div",{class:"explanation-header"},[t("span",{class:"explanation-icon"},"📘"),t("span",null,"شرح مفصّل")],-1)),t("div",{class:"explanation-content",innerHTML:S(e.lesson.explanation)},null,8,_d)])):g("",!0),(L=e.lesson.tips)!=null&&L.length?(i(),l("div",ud,[P[6]||(P[6]=t("div",{class:"tips-header"},[t("span",{class:"tips-icon"},"💡"),Q(" نصائح")],-1)),t("ul",null,[(i(!0),l(F,null,j(e.lesson.tips,(k,$)=>(i(),l("li",{key:$},v(k),1))),128))])])):g("",!0),(T=e.lesson.concepts)!=null&&T.length?(i(),l("div",pd,[(i(!0),l(F,null,j(e.lesson.concepts,k=>(i(),l("span",{key:k,class:"concept-chip",style:W({"--accent":e.levelColor})},v(k),5))),128))])):g("",!0)],64)):g("",!0),o.value===1?(i(),l(F,{key:1},[P[7]||(P[7]=t("div",{class:"section-header"},[t("span",null,"📝"),Q(" مثال تفاعلي")],-1)),E(Ye,{"initial-code":e.lesson.code},null,8,["initial-code"])],64)):g("",!0),o.value===2?(i(),l(F,{key:2},[t("div",{class:"exercise-card",style:W({"--accent":e.levelColor})},[P[8]||(P[8]=t("div",{class:"section-header"},[t("span",null,"🎯"),Q(" تمرين")],-1)),t("p",vd,v(e.lesson.exercise),1)],4),E(Ye,{"initial-code":h.value},null,8,["initial-code"]),e.lesson.hint?(i(),l("button",{key:0,class:"hint-btn",onClick:P[1]||(P[1]=k=>c.value=!c.value)},v(c.value?"إخفاء التلميح":"🔍 عرض تلميح"),1)):g("",!0),E(se,{name:"hint-reveal"},{default:y(()=>[c.value?(i(),l("div",hd,"💡 "+v(e.lesson.hint),1)):g("",!0)]),_:1})],64)):g("",!0)]))]}),_:1},8,["name"]),E(se,{name:"celebrate"},{default:y(()=>[d.value?(i(),l("div",fd,[...P[9]||(P[9]=[t("span",{class:"confetti"},"🎉",-1),t("span",{class:"celebrate-msg"},"أحسنت! أكملت الدرس",-1)])])):g("",!0)]),_:1})],512),E(Yr,{"lesson-id":e.lesson.id,"lesson-title":e.lesson.title,"section-id":e.sectionId,"level-id":e.levelId,"level-color":e.levelColor},null,8,["lesson-id","lesson-title","section-id","level-id","level-color"]),t("div",md,[e.lessonIndex>0?(i(),l("button",{key:0,class:"nav-btn ghost",onClick:P[2]||(P[2]=L=>V.$emit("prev"))},[...P[10]||(P[10]=[t("span",{class:"arrow"},"⟩",-1),Q(" الدرس السابق ",-1)])])):g("",!0),P[14]||(P[14]=t("div",{class:"spacer"},null,-1)),o.value>0?(i(),l("button",{key:1,class:"nav-btn secondary",onClick:M},[...P[11]||(P[11]=[t("span",{class:"arrow"},"⟩",-1),Q(" السابق ",-1)])])):g("",!0),o.value<2?(i(),l("button",{key:2,class:"nav-btn primary",style:W({"--bg":e.levelColor}),onClick:I},[...P[12]||(P[12]=[Q(" التالي ",-1),t("span",{class:"arrow"},"⟨",-1)])],4)):e.isCompleted?(i(),l("button",{key:4,class:"nav-btn primary",style:W({"--bg":e.levelColor}),onClick:P[3]||(P[3]=L=>V.$emit("next"))},[...P[13]||(P[13]=[Q(" الدرس التالي ",-1),t("span",{class:"arrow"},"⟨",-1)])],4)):(i(),l("button",{key:3,class:"nav-btn primary complete-btn",style:W({"--bg":e.levelColor}),onClick:H}," ✓ إكمال الدرس ",4))])],512))}}),Et=q(gd,[["__scopeId","data-v-30a80dc5"]]),xd={class:"ls-toggle"},kd={class:"ls-body"},bd={class:"ls-grid"},yd={class:"ls-card ls-card--ring"},$d={class:"ls-ring",viewBox:"0 0 80 80"},Sd=["stroke-dashoffset"],Cd={x:"40",y:"38",class:"ls-ring-pct"},Ld={x:"40",y:"50",class:"ls-ring-sub"},Pd={class:"ls-card"},Td={class:"ls-value"},Od={class:"ls-card"},Id={class:"ls-icon"},wd={class:"ls-value"},Ed={class:"ls-card"},qd={class:"ls-value ls-value--sm"},Vd={class:"ls-card"},Md={class:"ls-value"},dt="sad-learn-daily",Ad=w({__name:"LearningStats",props:{completedCount:{},totalLessons:{}},setup(e){const n=e,s=C(!1),a=C(0),o=C(0);function c(){return new Date().toISOString().slice(0,10)}function r(){try{return JSON.parse(localStorage.getItem(dt)||"{}")}catch{return{}}}function d(){const P=r();P[c()]=!0,localStorage.setItem(dt,JSON.stringify(P))}function _(){const P=r();if(!Object.keys(P).sort().reverse().length)return 0;let T=0;const k=new Date;for(let $=0;$<365;$++){const D=k.toISOString().slice(0,10);if(P[D])T++;else if($>0)break;k.setDate(k.getDate()-1)}return T}function p(){const P=r(),L=Object.keys(P).sort();if(!L.length)return 1;const T=new Date(L[0]);return Math.max(1,Math.ceil((new Date().getTime()-T.getTime())/864e5)+1)}const m=C(0),h=C(1),S=O(()=>n.totalLessons?Math.round(n.completedCount/n.totalLessons*100):0),f=2*Math.PI*36,b=O(()=>f-f*S.value/100),I=O(()=>{const P=S.value;return P>=95?"خبير":P>=75?"محترف":P>=50?"متقدم":P>=30?"متوسط":P>=10?"متعلم":"مبتدئ"}),M=O(()=>{const P=a.value*5;if(P<60)return`${P} دقيقة`;const L=Math.floor(P/60),T=P%60;return T?`${L} ساعة و ${T} د`:`${L} ساعة`}),H=O(()=>(a.value/h.value).toFixed(1));function V(P,L,T=800){const k=performance.now(),$=D=>{const K=Math.min((D-k)/T,1),G=1-Math.pow(1-K,3);L(Math.round(P*G)),K<1&&requestAnimationFrame($)};requestAnimationFrame($)}return ee(()=>{d(),m.value=_(),h.value=p(),V(n.completedCount,P=>a.value=P),V(m.value,P=>o.value=P)}),(P,L)=>(i(),l("div",{class:N(["ls-panel",{collapsed:s.value}])},[t("button",{class:"ls-header",onClick:L[0]||(L[0]=T=>s.value=!s.value)},[L[1]||(L[1]=t("span",{class:"ls-title"},"📊 إحصائيات التعلم",-1)),t("span",xd,v(s.value?"▸":"▾"),1)]),Re(t("div",kd,[t("div",bd,[t("div",yd,[(i(),l("svg",$d,[L[2]||(L[2]=t("circle",{cx:"40",cy:"40",r:"36",class:"ls-ring-bg"},null,-1)),t("circle",{cx:"40",cy:"40",r:"36",class:"ls-ring-fg","stroke-dasharray":f,"stroke-dashoffset":b.value,transform:"rotate(-90 40 40)"},null,8,Sd),t("text",Cd,v(S.value)+"%",1),t("text",Ld,v(a.value)+"/"+v(e.totalLessons),1)])),L[3]||(L[3]=t("div",{class:"ls-label"},"📚 دروس مكتملة",-1))]),t("div",Pd,[L[4]||(L[4]=t("div",{class:"ls-icon"},"🏆",-1)),t("div",Td,v(I.value),1),L[5]||(L[5]=t("div",{class:"ls-label"},"مستوى الإنجاز",-1))]),t("div",Od,[t("div",Id,v(o.value>3?"🔥":"📅"),1),t("div",wd,[Q(v(o.value)+" ",1),L[6]||(L[6]=t("span",{class:"ls-unit"},"يوم",-1))]),L[7]||(L[7]=t("div",{class:"ls-label"},"سلسلة التعلم",-1))]),t("div",Ed,[L[8]||(L[8]=t("div",{class:"ls-icon"},"⏱️",-1)),t("div",qd,v(M.value),1),L[9]||(L[9]=t("div",{class:"ls-label"},"وقت التعلم",-1))]),t("div",Vd,[L[10]||(L[10]=t("div",{class:"ls-icon"},"📈",-1)),t("div",Md,v(H.value),1),L[11]||(L[11]=t("div",{class:"ls-label"},"معدل يومي",-1))])])],512),[[Yt,!s.value]])],2))}}),qt=q(Ad,[["__scopeId","data-v-8d378785"]]),Vt="sad-learn-achievements",Dd=[{id:"lesson_1",title:"أول درس",description:"أكملت أول درس لك",icon:"🌱",category:"lesson"},{id:"lesson_10",title:"عشرة دروس",description:"أكملت 10 دروس",icon:"📘",category:"lesson"},{id:"lesson_50",title:"خمسون درساً",description:"أكملت 50 درساً",icon:"🔥",category:"lesson"},{id:"lesson_100",title:"مئة درس",description:"أكملت 100 درس",icon:"💯",category:"lesson"},{id:"lesson_200",title:"مئتا درس",description:"أكملت 200 درس",icon:"🏅",category:"lesson"},{id:"lesson_300",title:"أكملت الكل",description:"أكملت جميع الدروس الـ 300",icon:"👑",category:"lesson"},{id:"level_1",title:"إتمام المستوى ١",description:"أنهيت المستوى الأول بالكامل",icon:"⭐",category:"level"},{id:"level_2",title:"إتمام المستوى ٢",description:"أنهيت المستوى الثاني بالكامل",icon:"⭐",category:"level"},{id:"level_3",title:"إتمام المستوى ٣",description:"أنهيت المستوى الثالث بالكامل",icon:"🌟",category:"level"},{id:"level_4",title:"إتمام المستوى ٤",description:"أنهيت المستوى الرابع بالكامل",icon:"🌟",category:"level"},{id:"level_5",title:"إتمام المستوى ٥",description:"أنهيت المستوى الخامس بالكامل",icon:"💎",category:"level"},{id:"streak_1",title:"يوم واحد",description:"تعلّمت يوماً متواصلاً",icon:"✨",category:"streak"},{id:"streak_7",title:"أسبوع كامل",description:"تعلّمت 7 أيام متواصلة",icon:"🔥",category:"streak"},{id:"streak_30",title:"شهر كامل",description:"تعلّمت 30 يوماً متواصلاً",icon:"🏆",category:"streak"},{id:"speed_5",title:"سريع البرق",description:"أكملت 5 دروس في جلسة واحدة",icon:"⚡",category:"speed"}],_t={lesson_1:1,lesson_10:10,lesson_50:50,lesson_100:100,lesson_200:200,lesson_300:300},ut={streak_1:1,streak_7:7,streak_30:30},pt=C(!1);function we(){const e=typeof localStorage<"u"?localStorage.getItem(Vt):null,n={};if(e)try{const s=JSON.parse(e);for(const a of s)n[a.id]={unlocked:a.unlocked,unlockedAt:a.unlockedAt}}catch{}return Dd.map(s=>{var a,o;return{...s,unlocked:((a=n[s.id])==null?void 0:a.unlocked)??!1,unlockedAt:((o=n[s.id])==null?void 0:o.unlockedAt)??null}})}function Bd(e){typeof localStorage<"u"&&localStorage.setItem(Vt,JSON.stringify(e))}function Nd(){function e(){return we()}function n(){return we().filter(o=>o.unlocked).length}function s(){const o=we().filter(c=>c.unlocked&&c.unlockedAt);return o.length?o.reduce((c,r)=>r.unlockedAt>c.unlockedAt?r:c):null}function a(o,c,r,d){const _=we(),p=[],m=Date.now();for(const h of _){if(h.unlocked)continue;let S=!1;if(h.category==="lesson"&&_t[h.id]!=null)S=o>=_t[h.id];else if(h.category==="level"){const f=parseInt(h.id.replace("level_",""),10);S=!!c[f-1]}else h.category==="streak"&&ut[h.id]!=null?S=r>=ut[h.id]:h.id==="speed_5"&&(S=d>=5);S&&(h.unlocked=!0,h.unlockedAt=m,p.push(h))}return p.length&&(Bd(_),pt.value=!0),p}return{getAchievements:e,getUnlockedCount:n,getLatestAchievement:s,checkAchievements:a,hasNewAchievement:pt}}const Fd={class:"achievements-panel",dir:"rtl"},Rd={class:"achievements-header"},Hd={class:"count"},zd={key:0,class:"all-done"},Ud={class:"filter-tabs"},Gd=["onClick"],jd={class:"badges-grid"},Jd={class:"badge-icon"},Qd={class:"icon-emoji"},Wd={key:0,class:"lock-overlay"},Kd={class:"badge-info"},Yd={class:"badge-title"},Xd={key:0,class:"badge-date"},Zd=w({__name:"AchievementBadges",setup(e){const{getAchievements:n}=Nd(),s=C([]),a=C("all"),o=C(!1),c=[{key:"all",label:"الكل"},{key:"lesson",label:"الدروس"},{key:"level",label:"المستويات"},{key:"streak",label:"السلسلة"},{key:"speed",label:"السرعة"}],r=O(()=>a.value==="all"?s.value:s.value.filter(h=>h.category===a.value)),d=O(()=>s.value.filter(h=>h.unlocked).length),_=O(()=>s.value.length),p=O(()=>_.value>0&&d.value===_.value);function m(h){return h?new Date(h).toLocaleDateString("ar-EG",{year:"numeric",month:"short",day:"numeric"}):""}return ee(()=>{s.value=n(),requestAnimationFrame(()=>{o.value=!0})}),(h,S)=>(i(),l("div",Fd,[t("div",Rd,[t("h3",null,[S[0]||(S[0]=Q("🏆 الإنجازات ",-1)),t("span",Hd,"("+v(d.value)+" / "+v(_.value)+")",1)]),p.value?(i(),l("p",zd,"أحسنت! 🎉")):g("",!0)]),t("div",Ud,[(i(),l(F,null,j(c,f=>t("button",{key:f.key,class:N(["filter-btn",{active:a.value===f.key}]),onClick:b=>a.value=f.key},v(f.label),11,Gd)),64))]),t("div",jd,[(i(!0),l(F,null,j(r.value,(f,b)=>(i(),l("div",{key:f.id,class:N(["badge-card",{unlocked:f.unlocked,locked:!f.unlocked}]),style:W(f.unlocked&&o.value?`animation-delay: ${b*60}ms`:void 0)},[t("div",Jd,[t("span",Qd,v(f.icon),1),f.unlocked?g("",!0):(i(),l("span",Wd,"🔒"))]),t("div",Kd,[t("span",Yd,v(f.title),1),t("span",{class:N(["badge-desc",{hidden:!f.unlocked}])},v(f.description),3),f.unlocked&&f.unlockedAt?(i(),l("span",Xd,v(m(f.unlockedAt)),1)):g("",!0)])],6))),128))])]))}}),Mt=q(Zd,[["__scopeId","data-v-938dc5f9"]]),e_={class:"progress-track"},t_={class:"quiz-header"},n_={class:"counter"},s_={class:"quiz-title"},o_={class:"question-text"},i_={class:"options"},a_=["disabled","onClick"],l_={class:"option-num"},c_={class:"option-text"},r_={key:0,class:"mark"},d_={key:1,class:"mark"},__={key:0,class:"explanation"},u_={key:0,class:"celebration"},p_={key:1,class:"results"},v_={class:"ring-container"},h_={viewBox:"0 0 120 120",class:"ring-svg"},f_={class:"ring-pct"},m_={class:"score-line"},g_={class:"result-msg"},x_=w({__name:"QuizModal",props:{title:{},questions:{},levelColor:{}},emits:["close","complete"],setup(e,{emit:n}){const s=e,a=n,o=C(0),c=C(null),r=C(!1),d=C(0),_=C(!1),p=C("left"),m=C(!1),h=O(()=>s.questions[o.value]),S=O(()=>(o.value+(r.value?1:0))/s.questions.length*100),f=O(()=>Math.round(d.value/s.questions.length*100)),b=2*Math.PI*54,I=O(()=>b-b*f.value/100),M=O(()=>f.value===100?"ممتاز! 🎯 أتقنت هذا القسم":f.value>=70?"أحسنت! 👏 إنجاز رائع":f.value>=50?"جيد! 📚 راجع بعض الدروس":"حاول مجدداً 💪 المحاولة تصنع النجاح");function H(T){r.value||(c.value=T,r.value=!0,T===h.value.correct&&(d.value++,m.value=!0,setTimeout(()=>m.value=!1,700)))}function V(){o.value<s.questions.length-1?(p.value="left",o.value++,c.value=null,r.value=!1):(_.value=!0,a("complete",d.value,s.questions.length))}function P(T){return r.value?T===h.value.correct?"correct":T===c.value?"incorrect":"dimmed":""}function L(T){if(T.key==="Escape"){a("close");return}if(!_.value){if(!r.value&&T.key>="1"&&T.key<="4"){const k=parseInt(T.key)-1;k<h.value.options.length&&H(k)}r.value&&(T.key==="Enter"||T.key===" ")&&V()}}return ee(()=>{document.body.style.overflow="hidden",window.addEventListener("keydown",L)}),de(()=>{document.body.style.overflow="",window.removeEventListener("keydown",L)}),(T,k)=>(i(),A(kt,{to:"body"},[E(se,{name:"modal"},{default:y(()=>[t("div",{class:"quiz-overlay",onClick:k[2]||(k[2]=Te($=>a("close"),["self"]))},[t("div",{class:"quiz-card",style:W({"--accent":e.levelColor})},[t("button",{class:"close-btn",onClick:k[0]||(k[0]=$=>a("close")),"aria-label":"إغلاق"},"✕"),t("div",e_,[t("div",{class:"progress-fill",style:W({width:S.value+"%"})},null,4)]),_.value?(i(),l("div",p_,[k[4]||(k[4]=t("h2",{class:"results-heading"},"النتيجة",-1)),t("div",v_,[(i(),l("svg",h_,[k[3]||(k[3]=t("circle",{cx:"60",cy:"60",r:"54",class:"ring-bg"},null,-1)),t("circle",{cx:"60",cy:"60",r:"54",class:"ring-fg",style:W({strokeDasharray:b,strokeDashoffset:I.value,stroke:e.levelColor})},null,4)])),t("span",f_,v(f.value)+"%",1)]),t("p",m_,v(d.value)+" / "+v(e.questions.length),1),t("p",g_,v(M.value),1),t("button",{class:"close-result-btn",onClick:k[1]||(k[1]=$=>a("close"))},"إغلاق")])):(i(),l(F,{key:0},[t("div",t_,[t("span",n_,"السؤال "+v(o.value+1)+" من "+v(e.questions.length),1),t("h2",s_,v(e.title),1)]),E(Xt,{name:"slide-"+p.value,tag:"div",class:"question-area"},{default:y(()=>[(i(),l("div",{key:o.value,class:"question-block"},[t("p",o_,v(h.value.question),1),t("div",i_,[(i(!0),l(F,null,j(h.value.options,($,D)=>(i(),l("button",{key:D,class:N(["option-btn",P(D)]),disabled:r.value,onClick:K=>H(D)},[t("span",l_,v(D+1),1),t("span",c_,v($),1),r.value&&D===h.value.correct?(i(),l("span",r_,"✓")):r.value&&D===c.value&&D!==h.value.correct?(i(),l("span",d_,"✗")):g("",!0)],10,a_))),128))]),E(se,{name:"fade"},{default:y(()=>[r.value?(i(),l("div",__,[t("p",null,v(h.value.explanation),1)])):g("",!0)]),_:1}),E(se,{name:"fade"},{default:y(()=>[r.value?(i(),l("button",{key:0,class:"next-btn",onClick:V},v(o.value<e.questions.length-1?"التالي ←":"عرض النتيجة"),1)):g("",!0)]),_:1})]))]),_:1},8,["name"]),m.value?(i(),l("div",u_)):g("",!0)],64))],4)])]),_:1})]))}}),At=q(x_,[["__scopeId","data-v-d7efa81a"]]),k_={class:"notes-review-modal",dir:"rtl"},b_={class:"review-header"},y_={class:"header-actions"},$_={class:"review-toolbar"},S_={class:"search-box"},C_={class:"filter-tabs"},L_=["onClick"],P_={class:"tab-count"},T_={class:"review-content"},O_={class:"note-card-header"},I_={class:"note-info"},w_={key:0,class:"bookmark-star"},E_={class:"note-lesson-title"},q_={key:1,class:"note-level-badge"},V_={class:"note-actions"},M_=["onClick"],A_=["onClick","title"],D_=["onClick"],B_={key:0,class:"note-card-body"},N_={class:"note-text"},F_={class:"note-card-footer"},R_={class:"note-date"},H_={key:0,class:"note-chars"},z_={key:1,class:"empty-state"},U_={class:"empty-icon"},G_={class:"empty-title"},j_={class:"empty-desc"},J_={key:0,class:"review-footer"},Q_=w({__name:"NotesReview",props:{levelColor:{}},emits:["close","goToLesson"],setup(e,{emit:n}){const s=n,{getAllNotes:a,getBookmarkedNotes:o,getNotesCount:c,getBookmarksCount:r,toggleBookmark:d,deleteNote:_,exportNotes:p}=lt(),m=C(""),h=C("all"),S=C(!1),f=O(()=>a()),b=O(()=>o()),I=O(()=>c()),M=O(()=>r()),H=O(()=>[{key:"all",icon:"📝",label:"الكل",count:f.value.length},{key:"bookmarks",icon:"⭐",label:"المرجعية",count:b.value.length}]),V=O(()=>{let G;if(h.value==="bookmarks"?G=b.value:G=f.value,m.value.trim()){const J=m.value.trim().toLowerCase();G=G.filter(z=>z.content.toLowerCase().includes(J)||z.lessonTitle&&z.lessonTitle.toLowerCase().includes(J))}return G});function P(G){s("goToLesson",G.lessonId,G.levelId,G.sectionId),s("close")}function L(G){d(G.lessonId)}function T(G){confirm("هل تريد حذف هذه الملاحظة؟")&&_(G.lessonId)}function k(){const G=p();navigator.clipboard.writeText(G),S.value=!0,setTimeout(()=>{S.value=!1},2e3)}function $(G,J){return G.length>J?G.slice(0,J)+"...":G}function D(G){return new Date(G).toLocaleDateString("ar-SA",{year:"numeric",month:"short",day:"numeric",hour:"2-digit",minute:"2-digit"})}function K(G){G.key==="Escape"&&s("close")}return ee(()=>{document.body.style.overflow="hidden",document.addEventListener("keydown",K)}),de(()=>{document.body.style.overflow="",document.removeEventListener("keydown",K)}),(G,J)=>(i(),A(kt,{to:"body"},[t("div",{class:"notes-review-overlay",onClick:J[3]||(J[3]=Te(z=>G.$emit("close"),["self"]))},[t("div",k_,[t("div",b_,[J[4]||(J[4]=t("div",{class:"header-right"},[t("span",{class:"header-icon"},"📓"),t("h2",{class:"header-title"},"ملاحظاتي وإشاراتي المرجعية")],-1)),t("div",y_,[f.value.length>0?(i(),l("button",{key:0,class:"export-btn",onClick:k,title:"تصدير الملاحظات"},v(S.value?"✓ نُسخت":"📤 تصدير"),1)):g("",!0),t("button",{class:"close-btn",onClick:J[0]||(J[0]=z=>G.$emit("close")),title:"إغلاق"},"✕")])]),t("div",$_,[t("div",S_,[J[5]||(J[5]=t("span",{class:"search-icon"},"🔍",-1)),Re(t("input",{"onUpdate:modelValue":J[1]||(J[1]=z=>m.value=z),type:"text",class:"search-input",placeholder:"بحث في الملاحظات...",dir:"rtl"},null,512),[[et,m.value]]),m.value?(i(),l("button",{key:0,class:"clear-search",onClick:J[2]||(J[2]=z=>m.value="")},"✕")):g("",!0)]),t("div",C_,[(i(!0),l(F,null,j(H.value,z=>(i(),l("button",{key:z.key,class:N(["filter-tab",{active:h.value===z.key}]),style:W({"--accent":e.levelColor}),onClick:ie=>h.value=z.key},[t("span",null,v(z.icon),1),t("span",null,v(z.label),1),t("span",P_,v(z.count),1)],14,L_))),128))])]),t("div",T_,[V.value.length>0?(i(!0),l(F,{key:0},j(V.value,z=>(i(),l("div",{key:z.lessonId,class:"note-card"},[t("div",O_,[t("div",I_,[z.bookmarked?(i(),l("span",w_,"⭐")):g("",!0),t("span",E_,v(z.lessonTitle||z.lessonId),1),z.levelId?(i(),l("span",q_," المستوى "+v(z.levelId),1)):g("",!0)]),t("div",V_,[t("button",{class:"note-action-btn",onClick:ie=>P(z),title:"الذهاب للدرس"}," ← فتح ",8,M_),t("button",{class:N(["note-action-btn bookmark-toggle",{active:z.bookmarked}]),onClick:ie=>L(z),title:z.bookmarked?"إزالة الإشارة":"إضافة إشارة"},v(z.bookmarked?"⭐":"☆"),11,A_),t("button",{class:"note-action-btn delete-action",onClick:ie=>T(z),title:"حذف"}," 🗑️ ",8,D_)])]),z.content.trim()?(i(),l("div",B_,[t("p",N_,v($(z.content,300)),1)])):g("",!0),t("div",F_,[t("span",R_,v(D(z.updatedAt)),1),z.content.trim()?(i(),l("span",H_,v(z.content.length)+" حرف ",1)):g("",!0)])]))),128)):(i(),l("div",z_,[t("div",U_,v(h.value==="bookmarks"?"☆":"📝"),1),t("p",G_,v(m.value?"لا توجد نتائج":h.value==="bookmarks"?"لا توجد إشارات مرجعية بعد":"لا توجد ملاحظات بعد"),1),t("p",j_,v(m.value?"جرّب كلمات بحث مختلفة":"ابدأ بكتابة ملاحظاتك أثناء الدروس!"),1)]))]),f.value.length>0?(i(),l("div",J_,[t("span",null,"📝 "+v(I.value)+" ملاحظة",1),t("span",null,"⭐ "+v(M.value)+" إشارة مرجعية",1)])):g("",!0)])])]))}}),W_=q(Q_,[["__scopeId","data-v-5184b138"]]),K_=[{sectionId:"l1_s1",questions:[{question:"ما هي الدالة المستخدمة لطباعة نص في لغة ص؟",options:["اكتب()","اطبع()","أظهر()","نص()"],correct:1,explanation:"دالة اطبع() هي الدالة المدمجة لطباعة النصوص والقيم في لغة ص."},{question:"كيف نُعرّف متغيراً في لغة ص؟",options:["عرّف اسم = قيمة","متغير اسم = قيمة","اسم := قيمة","دع اسم = قيمة"],correct:1,explanation:'نستخدم الكلمة المفتاحية "متغير" لتعريف المتغيرات في لغة ص.'},{question:"أي من التالي تعليق صحيح في لغة ص؟",options:["// تعليق","/* تعليق */","# تعليق","-- تعليق"],correct:2,explanation:"التعليقات في لغة ص تبدأ بعلامة # للسطر الواحد."},{question:'ما الفرق بين "متغير" و"ثابت"؟',options:["لا فرق بينهما","المتغير يمكن تغيير قيمته، والثابت لا يمكن","الثابت أسرع من المتغير","المتغير للأرقام والثابت للنصوص"],correct:1,explanation:"الثابت (ثابت) يُعيّن مرة واحدة ولا يمكن تغيير قيمته لاحقاً."},{question:"ما هي أنواع البيانات الأساسية في لغة ص؟",options:["رقم، نص، منطقي فقط","رقم، عشري، نص، منطقي","صحيح، عائم، سلسلة","int، float، string، bool"],correct:1,explanation:"أنواع البيانات الأساسية: رقم (صحيح)، عشري (كسري)، نص، منطقي (صحيح/خطأ)."}]},{sectionId:"l1_s2",questions:[{question:"ما ناتج: 10 % 3 ؟",options:["3","3.33","1","0"],correct:2,explanation:"عامل % يُرجع باقي القسمة. 10 ÷ 3 = 3 والباقي 1."},{question:"ما ناتج: 2 ** 3 ؟",options:["6","8","5","9"],correct:1,explanation:"** عامل الأس. 2 أس 3 = 2 × 2 × 2 = 8."},{question:"ما ترتيب تنفيذ العمليات الصحيح؟",options:["من اليمين لليسار دائماً","الضرب والقسمة ثم الجمع والطرح","من اليسار لليمين دائماً","الأقواس أولاً ثم الأس ثم الضرب/القسمة ثم الجمع/الطرح"],correct:3,explanation:"ترتيب الأولويات: الأقواس > الأس > الضرب/القسمة/الباقي > الجمع/الطرح."},{question:"ماذا يفعل العامل += ؟",options:["يقارن قيمتين","يزيد المتغير بقيمة ويخزن النتيجة","ينشئ متغير جديد","يطبع القيمة"],correct:1,explanation:"س += 5 يعني س = س + 5. يختصر عملية الجمع والتعيين."},{question:"أي دالة تحوّل نصاً لرقم في لغة ص؟",options:["رقم()","إلى_رقم()","حوّل()","عدد()"],correct:1,explanation:"دالة إلى_رقم() تحوّل النص لرقم صحيح أو عشري."}]},{sectionId:"l1_s3",questions:[{question:"كيف ندمج نصين في لغة ص؟",options:["نص1 & نص2","نص1 + نص2","دمج(نص1، نص2)","نص1.أضف(نص2)"],correct:1,explanation:"عامل + يدمج النصوص عند استخدامه بين قيم نصية."},{question:"ما الدالة التي تُرجع طول النص؟",options:["حجم(نص)","طول(نص)","عدد(نص)","نص.طول"],correct:1,explanation:"دالة طول() تُرجع عدد أحرف النص."},{question:'ما ناتج: "مرحبا"[0] ؟',options:["م","ر","خطأ","ا"],correct:0,explanation:'الفهرسة تبدأ من 0. الحرف عند الفهرس 0 هو "م".'},{question:"أي عملية تبحث عن نص داخل نص آخر؟",options:["ابحث()","جد()","يتضمن()","يحتوي()"],correct:2,explanation:"دالة يتضمن() تتحقق من وجود نص فرعي داخل نص أكبر."},{question:"ما هي النصوص المُنسقة (Template Literals)؟",options:["نصوص بلغة أخرى","نصوص تحتوي تعبيرات متغيرة بداخلها","نصوص مشفرة","نصوص بدون أحرف خاصة"],correct:1,explanation:"النصوص المنسقة تدمج قيم المتغيرات والتعبيرات مباشرة داخل النص."}]},{sectionId:"l1_s4",questions:[{question:"ما الكلمة المفتاحية لبدء شرط في لغة ص؟",options:["لو","عندما","إذا","حالة"],correct:2,explanation:'نستخدم "إذا" لبدء جملة شرطية في لغة ص.'},{question:"كيف نختم كتلة الشرط في لغة ص؟",options:["}","نهاية_إذا","نهاية","إنهاء"],correct:2,explanation:'جميع الكتل في لغة ص تنتهي بكلمة "نهاية".'},{question:"ما ناتج: صحيح و خطأ ؟",options:["صحيح","خطأ","لاشيء","خطأ برمجي"],correct:1,explanation:'العامل "و" (AND) يُرجع صحيح فقط إذا كان الطرفان صحيحين.'},{question:"ما هو العامل الثلاثي في لغة ص؟",options:["إذا...وإلا","شرط ? قيمة1 : قيمة2","طابق...عندما","اختر...من"],correct:1,explanation:"العامل الثلاثي ? : يختصر جملة إذا/وإلا في تعبير واحد."},{question:'أي من عوامل المقارنة التالية يعني "لا يساوي"؟',options:["<>","!==","!=","=/="],correct:2,explanation:"العامل != يتحقق من عدم التساوي بين قيمتين."}]},{sectionId:"l1_s5",questions:[{question:"ما الكلمة المفتاحية لحلقة التكرار بشرط؟",options:["كرر","بينما","طالما","دام"],correct:1,explanation:'حلقة "بينما" تكرر طالما الشرط صحيح.'},{question:'ما الفرق بين "توقف" و"استمر"؟',options:["لا فرق بينهما","توقف تخرج من الحلقة، واستمر تتخطى الدورة الحالية","توقف تتخطى، واستمر تخرج","كلاهما يوقف البرنامج"],correct:1,explanation:"توقف (break) تُنهي الحلقة كلياً، واستمر (continue) تنتقل للدورة التالية."},{question:"ما الصيغة الصحيحة لحلقة لكل؟",options:["لكل عنصر من قائمة","لكل عنصر في قائمة","لكل (عنصر، قائمة)","كرر عنصر في قائمة"],correct:1,explanation:'الصيغة الصحيحة: "لكل عنصر في قائمة" ثم الكود ثم "نهاية".'},{question:"ما ناتج: لكل ي في مدى(1، 4) — كم مرة تتكرر؟",options:["3 مرات","4 مرات","5 مرات","مرتان"],correct:0,explanation:"مدى(1، 4) ينتج الأرقام 1، 2، 3 — ثلاث مرات (النهاية غير مشمولة)."},{question:"ما هي الحلقة المتداخلة؟",options:["حلقة بدون شرط","حلقة داخل حلقة أخرى","حلقة تكرر مرة واحدة","حلقة بشرطين"],correct:1,explanation:"الحلقة المتداخلة هي حلقة تكرار موجودة داخل حلقة تكرار أخرى."}]}],Y_=[{sectionId:"l2_s1",questions:[{question:"كيف تُعرّف دالة في لغة ص؟",options:["عرّف اسم()","دالة اسم()","وظيفة اسم()","إنشاء اسم()"],correct:1,explanation:'نستخدم الكلمة المفتاحية "دالة" لتعريف الدوال في لغة ص.'},{question:"ما الكلمة المفتاحية لإرجاع قيمة من دالة؟",options:["أعد","ارجع","أخرج","قيمة"],correct:1,explanation:'"ارجع" تُستخدم لإرجاع قيمة من الدالة إلى المستدعي.'},{question:"ما هي القيمة الافتراضية للمعامل؟",options:["قيمة يجب تمريرها دائماً","قيمة تُستخدم عند عدم تمرير وسيط","أول قيمة في المصفوفة","قيمة ثابتة لا تتغير"],correct:1,explanation:"القيمة الافتراضية تُستخدم تلقائياً إذا لم يُمرر وسيط عند استدعاء الدالة."},{question:"ما الفرق بين النطاق المحلي والعام؟",options:["لا فرق","المحلي داخل الدالة فقط، والعام في كل البرنامج","المحلي أسرع","العام للأرقام فقط"],correct:1,explanation:"المتغير المحلي يوجد فقط داخل الدالة، بينما العام يمكن الوصول إليه من أي مكان."},{question:"ما هي الدالة التكرارية (Recursive)؟",options:["دالة تستدعي دالة أخرى","دالة تستدعي نفسها","دالة بدون معاملات","دالة تُرجع مصفوفة"],correct:1,explanation:"الدالة التكرارية تستدعي نفسها مع حالة أساس لإيقاف التكرار."}]},{sectionId:"l2_s2",questions:[{question:"كيف تُنشئ مصفوفة فارغة في لغة ص؟",options:["متغير م = {}","متغير م = []","متغير م = ()","متغير م = مصفوفة()"],correct:1,explanation:"الأقواس المربعة [] تُنشئ مصفوفة فارغة."},{question:"كيف تصل للعنصر الثالث في المصفوفة؟",options:["م[3]","م[2]","م(3)","م.عنصر(3)"],correct:1,explanation:"الفهرسة تبدأ من 0، فالعنصر الثالث في الفهرس 2."},{question:"ما الفرق بين += و= عند إضافة عنصر لمصفوفة؟",options:["لا فرق","+= تُضيف دون إنشاء مصفوفة جديدة","+= ترمي خطأ","+= تُنشئ نسخة"],correct:1,explanation:"م += [عنصر] تُضيف العنصر للمصفوفة الحالية."},{question:"ما الناتج عند الوصول لفهرس خارج حدود المصفوفة؟",options:["0","لاشيء","خطأ في التشغيل","نص فارغ"],correct:2,explanation:"الوصول لفهرس خارج حدود المصفوفة يُسبب خطأ في وقت التشغيل."},{question:"ما هي المصفوفة المتداخلة؟",options:["مصفوفة كبيرة","مصفوفة تحتوي مصفوفات أخرى","مصفوفة مرتبة","مصفوفة فارغة"],correct:1,explanation:"المصفوفة المتداخلة تحتوي مصفوفات كعناصر، مثل [[1,2],[3,4]]."}]},{sectionId:"l2_s3",questions:[{question:"ما الدالة التي تقسم نصاً إلى مصفوفة؟",options:["افصل()","قسّم()","قطّع()","اقسم()"],correct:1,explanation:"دالة قسّم() تقسم النص حسب فاصل محدد وتُرجع مصفوفة."},{question:"ما الدالة التي تجمع عناصر مصفوفة في نص واحد؟",options:["ادمج()","اربط()","اجمع()","صِل()"],correct:0,explanation:"دالة ادمج() تجمع عناصر المصفوفة في نص واحد بفاصل."},{question:"كيف تزيل المسافات من بداية ونهاية نص؟",options:["نظّف()","قصّ()","أزل_مسافات()","تقليم()"],correct:1,explanation:"دالة قصّ() (trim) تزيل المسافات والأحرف البيضاء من الطرفين."},{question:"ما هي التعبيرات النمطية (Regex) في سياق النصوص؟",options:["نصوص عادية","أنماط للبحث والمطابقة في النصوص","نصوص مشفرة","تعليقات توثيقية"],correct:1,explanation:"التعبيرات النمطية أنماط خاصة تصف شكل النص المطلوب مطابقته."},{question:'ما ناتج استبدال "مرحبا" في "مرحبا يا عالم" بـ "أهلاً"؟',options:["مرحبا يا عالم","أهلاً يا عالم","أهلاً","خطأ"],correct:1,explanation:"دالة الاستبدال تستبدل النص المطابق وتُرجع النص الجديد."}]},{sectionId:"l2_s4",questions:[{question:"ما هي الخريطة (Map) في لغة ص؟",options:["مصفوفة مرتبة","هيكل بيانات يربط مفاتيح بقيم","دالة رياضية","نوع من الحلقات"],correct:1,explanation:"الخريطة تخزن أزواج مفتاح-قيمة للوصول السريع بالمفتاح."},{question:"كيف تُنشئ خريطة في لغة ص؟",options:["متغير خ = []","متغير خ = {}","متغير خ = خريطة()","متغير خ = ()"],correct:1,explanation:"الأقواس المعقوفة {} مع أزواج مفتاح:قيمة تُنشئ خريطة."},{question:"ماذا يحدث عند الوصول لمفتاح غير موجود في خريطة؟",options:["0","لاشيء","خطأ","نص فارغ"],correct:1,explanation:'عند الوصول لمفتاح غير موجود تُرجع "لاشيء" (null).'},{question:"كيف تتحقق من وجود مفتاح في خريطة؟",options:["مفتاح == لاشيء","خريطة.يوجد(مفتاح)","خريطة[مفتاح] != لاشيء","كل ما سبق صحيح"],correct:2,explanation:"يمكن التحقق بمقارنة القيمة بـ لاشيء لمعرفة وجود المفتاح."},{question:"كيف تحذف مفتاحاً من خريطة؟",options:["خريطة.احذف(مفتاح)","خريطة[مفتاح] = لاشيء","احذف خريطة[مفتاح]","خريطة -= مفتاح"],correct:1,explanation:"تعيين قيمة المفتاح إلى لاشيء يحذفه فعلياً من الخريطة."}]},{sectionId:"l2_s5",questions:[{question:"ما الكلمة المفتاحية لالتقاط الأخطاء؟",options:["التقط","امسك","اصطد","عالج"],correct:1,explanation:'نستخدم "حاول...امسك" لالتقاط الأخطاء ومعالجتها.'},{question:"ما الكلمة المفتاحية لرمي خطأ يدوياً؟",options:["خطأ","ارمي","أطلق","أنشئ_خطأ"],correct:1,explanation:'"ارمي" تُنشئ خطأً وتوقف التنفيذ العادي.'},{question:'متى يتم تنفيذ كتلة "أخيراً"؟',options:["فقط عند حدوث خطأ","فقط عند عدم حدوث خطأ","دائماً، سواء حدث خطأ أم لا","لا تُنفذ أبداً"],correct:2,explanation:'كتلة "أخيراً" تُنفذ دائماً بعد حاول/امسك بغض النظر عن النتيجة.'},{question:"لماذا لا يجب تجاهل الأخطاء؟",options:["لأنها تُبطئ البرنامج","لأنها قد تسبب سلوكاً غير متوقع وتسريب بيانات","لأنها تزيد حجم الملف","لأنها ممنوعة نحوياً"],correct:1,explanation:"تجاهل الأخطاء قد يسبب تسريب موارد وسلوك غير متوقع في البرنامج."},{question:"ما الفرق بين الخطأ والاستثناء؟",options:["لا فرق","الخطأ خارجي والاستثناء داخلي","الخطأ قاتل مثل نفاد الذاكرة، والاستثناء قابل للمعالجة","الاستثناء أسرع"],correct:2,explanation:"الأخطاء القاتلة (مثل نفاد الذاكرة) لا يمكن معالجتها، بينما الاستثناءات يمكن التقاطها."}]},{sectionId:"l2_s6",questions:[{question:"ما أهمية التعامل مع الملفات في البرمجة؟",options:["لتسريع البرنامج","لحفظ البيانات بشكل دائم بعد إغلاق البرنامج","لتجميل الكود","لتقليل الأخطاء"],correct:1,explanation:"الملفات تحفظ البيانات بشكل دائم على القرص لاستخدامها لاحقاً."},{question:"ما الفرق بين قراءة ملف نصي وملف ثنائي؟",options:["لا فرق","النصي يُقرأ كأحرف، والثنائي كبايتات","الثنائي أصغر حجماً","النصي أسرع"],correct:1,explanation:"الملفات النصية تحتوي أحرف مقروءة، والثنائية بيانات خام (بايتات)."},{question:"لماذا يجب إغلاق الملف بعد الانتهاء منه؟",options:["لتوفير مساحة القرص","لتحرير موارد النظام وضمان حفظ البيانات","لأن اللغة تتطلب ذلك","لتسريع البرنامج"],correct:1,explanation:"إغلاق الملف يحرر الموارد ويضمن كتابة البيانات المخزنة مؤقتاً."},{question:"ما صيغة JSON المستخدمة لتبادل البيانات؟",options:["قوائم وأرقام فقط","أزواج مفتاح/قيمة ومصفوفات","نصوص فقط","جداول بيانات"],correct:1,explanation:"JSON يدعم كائنات (مفتاح/قيمة) ومصفوفات وأرقام ونصوص ومنطقية."},{question:"ما أفضل ممارسة لقراءة ملفات كبيرة؟",options:["قراءة الملف كاملاً مرة واحدة","قراءته سطراً بسطر لتوفير الذاكرة","نسخه لملف أصغر","ضغطه قبل القراءة"],correct:1,explanation:"القراءة سطراً بسطر تمنع استهلاك الذاكرة عند التعامل مع ملفات ضخمة."}]}],X_=[{sectionId:"l3_classes_intro",questions:[{question:"ما الكلمة المفتاحية لإنشاء صنف في لغة ص؟",options:["كائن","صنف","نوع","قالب"],correct:1,explanation:'نستخدم "صنف" لتعريف نوع جديد يحتوي خصائص ودوال.'},{question:"ما هو الباني (Constructor)؟",options:["دالة عادية","دالة تُستدعى تلقائياً عند إنشاء كائن جديد","نوع بيانات","عامل حسابي"],correct:1,explanation:'الباني دالة خاصة تُنفذ تلقائياً عند إنشاء كائن بـ "جديد".'},{question:'ما وظيفة الكلمة "هذا" داخل الصنف؟',options:["الإشارة للصنف نفسه","الإشارة للكائن الحالي","إنشاء متغير محلي","استدعاء الباني"],correct:1,explanation:'"هذا" تشير للكائن الحالي الذي يتم التعامل معه.'},{question:"كيف تُنشئ كائناً جديداً من صنف؟",options:["صنف.أنشئ()","أنشئ صنف()","جديد صنف()","صنف()"],correct:2,explanation:'نستخدم "جديد" متبوعاً باسم الصنف لإنشاء كائن.'},{question:"ما الفرق بين الخاصية والدالة في الصنف؟",options:["لا فرق","الخاصية تخزن بيانات والدالة تنفذ عملية","الخاصية أسرع","الدالة لا تُرجع قيمة"],correct:1,explanation:"الخصائص تخزن حالة الكائن بينما الدوال تُعرّف سلوكه."}]},{sectionId:"l3_adv_classes",questions:[{question:'ما معنى مستوى الوصول "خاص"؟',options:["يمكن الوصول من أي مكان","يمكن الوصول فقط من داخل الصنف","يمكن الوصول من الأصناف الأبناء","يمكن الوصول من نفس الملف"],correct:1,explanation:'"خاص" يمنع الوصول للعضو من خارج الصنف — فقط الدوال الداخلية.'},{question:"ما هو تحميل العوامل الزائد (Operator Overloading)؟",options:["إنشاء عوامل جديدة","تعريف سلوك العوامل (+، -، ...) لأصناف مخصصة","حذف عوامل من اللغة","تسريع العمليات الحسابية"],correct:1,explanation:"تحميل العوامل يتيح تعريف ما يعنيه + أو == لكائنات صنفك."},{question:"ما الفرق بين الدالة العادية والدالة الساكنة (static)؟",options:["لا فرق","الساكنة تحتاج كائناً والعادية لا","الساكنة تنتمي للصنف نفسه ولا تحتاج كائناً","الساكنة أبطأ"],correct:2,explanation:"الدالة الساكنة تُستدعى على الصنف مباشرة بدون إنشاء كائن."},{question:"ما هي الخاصية (Property) بـ getter/setter؟",options:["متغير عادي","عضو يُتحكم بالقراءة والكتابة عبر دوال","ثابت","مصفوفة خاصة"],correct:1,explanation:"الخصائص مع احصل/عيّن تتيح التحقق من القيم عند القراءة والكتابة."},{question:"متى نستخدم الدالة toString في الصنف؟",options:["عند الحذف","عند تحويل الكائن لنص","عند المقارنة","عند الإنشاء فقط"],correct:1,explanation:"toString تُعرّف كيف يظهر الكائن عند طباعته أو تحويله لنص."}]},{sectionId:"l3_inheritance",questions:[{question:"ما الكلمة المفتاحية للوراثة في لغة ص؟",options:["من","يمتد","يرث","يأخذ"],correct:2,explanation:'نستخدم "يرث" لجعل صنف يرث من صنف آخر.'},{question:'ما معنى "تعدد الأشكال" (Polymorphism)؟',options:["صنف يرث من عدة أصناف","كائنات مختلفة تستجيب لنفس الرسالة بطرق مختلفة","إنشاء عدة كائنات","تغيير شكل المتغير"],correct:1,explanation:"تعدد الأشكال يتيح لأصناف مختلفة تنفيذ نفس الدالة بطريقتها الخاصة."},{question:"كيف تستدعي باني الصنف الأب؟",options:["أب.باني()","الأساس()","باني_أعلى()","وارث()"],correct:1,explanation:'"الأساس()" تستدعي باني الصنف الأب لتهيئة خصائصه.'},{question:"ما هو الصنف المجرد (Abstract)؟",options:["صنف بدون دوال","صنف لا يمكن إنشاء كائن منه مباشرة","صنف فارغ","صنف ساكن"],correct:1,explanation:"الصنف المجرد يُعرّف واجهة يجب على الأصناف الأبناء تنفيذها."},{question:"ما الفرق بين الوراثة والتركيب (Composition)؟",options:["لا فرق",'الوراثة "هو نوع من"، والتركيب "يحتوي على"',"التركيب أبطأ","الوراثة لا تدعم إعادة الاستخدام"],correct:1,explanation:'الوراثة علاقة "is-a" والتركيب علاقة "has-a". التركيب أكثر مرونة.'}]},{sectionId:"l3_structs_enums",questions:[{question:"ما الفرق بين البنية والصنف في لغة ص؟",options:["لا فرق","البنية لبيانات بسيطة بدون سلوك، والصنف كامل","البنية أسرع","الصنف لا يدعم الوراثة"],correct:1,explanation:"البنية (بنية) لتجميع بيانات مرتبطة بدون دوال معقدة."},{question:"ما هو التعداد (Enum)؟",options:["مصفوفة أرقام","مجموعة قيم ثابتة محددة مسبقاً","نوع من الحلقات","متغير خاص"],correct:1,explanation:"التعداد يُعرّف مجموعة ثابتة من القيم المسماة مثل أيام الأسبوع."},{question:"متى نفضّل البنية على الصنف؟",options:["دائماً","عندما نحتاج فقط تجميع بيانات بسيطة","عندما نحتاج وراثة","عندما نحتاج تعدد أشكال"],correct:1,explanation:"البنية مناسبة لحاويات بيانات بسيطة بدون سلوك معقد."},{question:"هل يمكن للتعداد حمل قيم مرتبطة؟",options:["لا أبداً","نعم، يمكن لكل حالة حمل بيانات","فقط أرقام","فقط نصوص"],correct:1,explanation:"التعدادات المتقدمة يمكن أن تحمل كل حالة بيانات مرتبطة مختلفة."},{question:"ما الفرق بين مطابقة التعداد واستخدام إذا/وإلا؟",options:["لا فرق","المطابقة تضمن تغطية جميع الحالات","إذا/وإلا أسرع","المطابقة للأرقام فقط"],correct:1,explanation:"مطابقة الأنماط مع طابق/عندما تضمن معالجة كل حالة ممكنة."}]},{sectionId:"l3_pattern_matching",questions:[{question:"ما الكلمة المفتاحية لمطابقة الأنماط في لغة ص؟",options:["قارن","طابق","اختر","حالة"],correct:1,explanation:'"طابق" تبدأ كتلة مطابقة الأنماط مع حالات "عندما".'},{question:"ما النمط الشامل (Wildcard) في المطابقة؟",options:["*","?","_","..."],correct:2,explanation:"النمط _ يطابق أي قيمة ويُستخدم كحالة افتراضية."},{question:"ما هو نمط النطاق في المطابقة؟",options:["قائمة قيم","مطابقة مجال أرقام مثل 1..10","مطابقة نص","مطابقة نوع"],correct:1,explanation:"نمط النطاق 1..10 يطابق أي رقم بين 1 و10."},{question:'ما الفرق بين "عندما" و"افتراضي" في طابق؟',options:["لا فرق","عندما تطابق قيمة محددة وافتراضي يطابق الباقي","افتراضي أسرع","عندما اختيارية وافتراضي إجبارية"],correct:1,explanation:'"افتراضي" تُنفذ عندما لا تتطابق أي حالة "عندما" أخرى.'},{question:"هل يمكن مطابقة عدة أنماط في حالة واحدة؟",options:["لا","نعم باستخدام ||","نعم باستخدام و","نعم باستخدام ،"],correct:1,explanation:"عندما 1 || 2 || 3: تطابق أي قيمة من القيم المذكورة."}]},{sectionId:"l3_advanced_functions",questions:[{question:"ما هي دالة اللامدا (Lambda)؟",options:["دالة كبيرة","دالة مجهولة بدون اسم","دالة ساكنة","دالة تكرارية"],correct:1,explanation:"اللامدا دالة مجهولة تُعرّف في مكانها وتُستخدم كتعبير."},{question:"ما هو المولد (Generator)؟",options:["أداة لتوليد أكواد","دالة تنتج قيماً واحدة تلو الأخرى بتكاسل","أداة إنشاء ملفات","نوع من الأصناف"],correct:1,explanation:"المولد ينتج القيم عند الطلب بدلاً من إنشائها كلها مسبقاً."},{question:"ما الفرق بين الدالة النقية وغير النقية؟",options:["لا فرق","النقية لا تعدل حالة خارجية وتُرجع نفس النتيجة لنفس المدخلات","النقية أسرع","غير النقية أفضل"],correct:1,explanation:"الدالة النقية بدون تأثيرات جانبية — نفس المدخل يعطي نفس المخرج دائماً."},{question:"ما هو السلسلة (Chaining) في استدعاء الدوال؟",options:["استدعاء دوال متعددة في سلسلة واحدة","تكرار الدالة","ربط ملفات","نوع من المصفوفات"],correct:0,explanation:"السلسلة تتيح: كائن.دالة1().دالة2().دالة3() في سطر واحد."},{question:"ما هي الدوال من الرتبة العليا (Higher-Order)؟",options:["دوال كبيرة","دوال تأخذ أو تُرجع دوالاً أخرى","دوال رياضية","دوال ساكنة"],correct:1,explanation:"دالة الرتبة العليا تقبل دوالاً كمعاملات أو تُرجع دوالاً."}]},{sectionId:"l3_modules",questions:[{question:"ما الكلمة المفتاحية لاستيراد وحدة في لغة ص؟",options:["حمّل","استورد","أضف","ضمّن"],correct:1,explanation:'"استورد" تُستخدم لإحضار وحدة أو عناصر منها.'},{question:"ما الكلمة المفتاحية لتصدير عنصر من وحدة؟",options:["أرسل","اعرض","صدّر","شارك"],correct:2,explanation:'"صدّر" تجعل الدالة أو الصنف متاحاً للاستيراد من وحدات أخرى.'},{question:"ما فائدة تقسيم الكود إلى وحدات؟",options:["تسريع البرنامج","تنظيم الكود وإعادة الاستخدام وتقليل التعقيد","تقليل حجم الملف","منع الأخطاء تماماً"],correct:1,explanation:"الوحدات تنظم الكود منطقياً وتسهّل الصيانة وإعادة الاستخدام."},{question:"ما هو فضاء الأسماء (Namespace)؟",options:["مجلد ملفات","حاوية منطقية لتجنب تعارض الأسماء","نوع بيانات","متغير خاص"],correct:1,explanation:"فضاء الأسماء ينظم العناصر ويمنع تعارض الأسماء بين الوحدات."},{question:"ما هو الاسم المستعار (Alias) عند الاستيراد؟",options:["حذف الوحدة","إعادة تسمية العنصر المستورد باسم مختصر","نسخ الملف","تعطيل الوحدة"],correct:1,explanation:'"كـ" تُستخدم لإعطاء اسم مختصر: استورد طويل_جداً كـ قصير.'}]},{sectionId:"l3_async",questions:[{question:"ما معنى البرمجة غير المتزامنة؟",options:["تنفيذ الكود بالتوازي","تنفيذ عمليات بدون انتظار اكتمال كل عملية قبل البدء بالتالية","البرمجة الكائنية","كتابة كود بدون أخطاء"],correct:1,explanation:"غير المتزامنة تتيح تنفيذ عمليات طويلة دون تعطيل بقية البرنامج."},{question:"ما الكلمة المفتاحية لتنفيذ عملية غير متزامنة في لغة ص؟",options:["موازي","غير_متزامن","خلفي","متعدد"],correct:1,explanation:'"غير_متزامن" تُعلّم الدالة كدالة غير متزامنة يمكن انتظارها.'},{question:'ما وظيفة "انتظر" في البرمجة غير المتزامنة؟',options:["إيقاف البرنامج","الانتظار حتى اكتمال عملية غير متزامنة","إنشاء مؤقت","تكرار عملية"],correct:1,explanation:'"انتظر" توقف تنفيذ الدالة حتى تكتمل العملية غير المتزامنة.'},{question:"ما هو الوعد (Promise)؟",options:["نوع بيانات","كائن يمثل نتيجة عملية ستكتمل مستقبلاً","متغير ثابت","دالة خاصة"],correct:1,explanation:"الوعد يمثل قيمة قد تكون متاحة الآن أو لاحقاً أو أبداً."},{question:"متى نستخدم البرمجة غير المتزامنة؟",options:["دائماً","عند عمليات الشبكة والملفات وقواعد البيانات","للعمليات الحسابية فقط","لتقليل الأخطاء"],correct:1,explanation:"غير المتزامنة ضرورية لعمليات I/O البطيئة كالشبكة والملفات."}]}],Z_=[{sectionId:"l4_design_patterns",questions:[{question:"ما هو نمط المفرد (Singleton)؟",options:["كائن بخاصية واحدة","نمط يضمن وجود كائن واحد فقط من الصنف","صنف بدون باني","دالة تُستدعى مرة واحدة"],correct:1,explanation:"Singleton يضمن إنشاء كائن واحد فقط ويوفر نقطة وصول عالمية."},{question:"ما هو نمط المراقب (Observer)؟",options:["مراقبة الأخطاء","نمط يُشعر المستمعين تلقائياً عند تغيّر الحالة","أداة تصحيح","نظام أمان"],correct:1,explanation:"Observer/Pub-Sub يفصل المُرسل عن المستقبل لتعديل مستقل."},{question:"متى نستخدم نمط المصنع (Factory)؟",options:["دائماً","عندما نريد إنشاء كائنات بدون تحديد الصنف الدقيق مسبقاً","عند حذف الكائنات","عند الطباعة فقط"],correct:1,explanation:"المصنع يُخفي تفاصيل الإنشاء ويُرجع الكائن المناسب حسب المدخلات."},{question:"ما هو نمط الاستراتيجية (Strategy)؟",options:["خطة للمشروع","تعريف عائلة خوارزميات واختيار واحدة وقت التشغيل","ترتيب الدوال","نمط للاختبارات"],correct:1,explanation:"Strategy يغلّف كل خوارزمية في صنف ويتيح التبديل بينها."},{question:"ما هو نمط الزخرفة (Decorator)؟",options:["تجميل واجهة المستخدم","إضافة وظائف جديدة لكائن بدون تعديل صنفه","حذف خصائص","ترتيب الكود"],correct:1,explanation:"Decorator يغلّف كائناً بطبقات تضيف وظائف دون تغيير الأصل."}]},{sectionId:"l4_data_structures",questions:[{question:"ما تعقيد البحث في جدول التجزئة (Hash Table)؟",options:["O(n)","O(log n)","O(1) في المتوسط","O(n²)"],correct:2,explanation:"التجزئة تحوّل المفتاح لفهرس مباشرة — O(1) غالباً."},{question:"ما هي الشجرة الثنائية (Binary Tree)؟",options:["قائمة من عنصرين","هيكل حيث كل عقدة لها فرعان كحد أقصى","مصفوفة بعدين","خريطة بمفتاحين"],correct:1,explanation:"الشجرة الثنائية كل عقدة فيها تحمل قيمة ولها فرع أيسر وأيمن."},{question:"ما الفرق بين المكدّس والطابور؟",options:["لا فرق","المكدّس LIFO (آخر داخل أول خارج) والطابور FIFO (أول داخل أول خارج)","المكدّس أسرع","الطابور للأرقام فقط"],correct:1,explanation:"المكدّس كومة أطباق (LIFO)، والطابور طابور بنك (FIFO)."},{question:"ما هو الرسم البياني (Graph)؟",options:["رسم جرافيكي","مجموعة عقد (vertices) وأضلع (edges) تربطها","نوع من المصفوفات","جدول بيانات"],correct:1,explanation:"الرسم البياني يمثل العلاقات بين العناصر عبر عقد وأضلع."},{question:"متى نستخدم القائمة المترابطة بدلاً من المصفوفة؟",options:["دائماً","عند الإدراج والحذف المتكرر في الوسط","عند البحث العشوائي","عند الحاجة لذاكرة أقل"],correct:1,explanation:"القائمة المترابطة تتفوق في الإدراج/الحذف O(1) مقابل O(n) للمصفوفة."}]},{sectionId:"l4_algorithms",questions:[{question:"ما تعقيد الترتيب السريع (Quick Sort) في أفضل حالة؟",options:["O(n)","O(n log n)","O(n²)","O(log n)"],correct:1,explanation:"Quick Sort في أفضل ومتوسط الحالات O(n log n)."},{question:'ما هو مبدأ "فرّق تسُد" (Divide and Conquer)؟',options:["حل المشكلة كاملة مرة واحدة","تقسيم المشكلة لأجزاء، حل كل جزء، ودمج الحلول","تجاهل الأجزاء الصعبة","استخدام عدة لغات"],correct:1,explanation:"Divide and Conquer يقسم المشكلة الكبيرة لمشاكل صغيرة يسهل حلها."},{question:"ما هي البرمجة الديناميكية (Dynamic Programming)؟",options:["برمجة سريعة","تخزين نتائج المسائل الفرعية لتجنب إعادة حسابها","برمجة الألعاب","تعديل الكود أثناء التشغيل"],correct:1,explanation:"DP تحفظ نتائج المسائل الفرعية (Memoization) لتسريع الحل."},{question:"ما تعقيد البحث الثنائي (Binary Search)؟",options:["O(n)","O(1)","O(log n)","O(n²)"],correct:2,explanation:"البحث الثنائي ينصّف مساحة البحث في كل خطوة — O(log n)."},{question:"ما هي الخوارزمية الجشعة (Greedy)؟",options:["خوارزمية بطيئة","تختار الأفضل محلياً في كل خطوة","تجرب كل الاحتمالات","تعمل فقط مع المصفوفات"],correct:1,explanation:"الجشعة تختار الخيار الأفضل الآن بدون النظر للمستقبل."}]},{sectionId:"l4_text_processing",questions:[{question:"ما هو التعبير النمطي (Regex)؟",options:["نمط تصميم","لغة لوصف أنماط في النصوص","دالة ترتيب","نوع ملف"],correct:1,explanation:"Regex لغة مختصرة لوصف ومطابقة أنماط نصية."},{question:"ما هو محلل النصوص (Parser)؟",options:["برنامج قراءة","برنامج يحلل نصاً ويحوّله لبنية بيانات","أداة ترجمة","مكتبة رسوميات"],correct:1,explanation:"المحلل يقرأ نصاً خاماً ويحوّله لهيكل منظم قابل للمعالجة."},{question:"ما هي خوارزمية KMP للبحث في النصوص؟",options:["بحث خطي عادي","خوارزمية بحث متقدمة تتجنب إعادة المقارنات","خوارزمية ترتيب","خوارزمية تشفير"],correct:1,explanation:"KMP تحلل النمط مسبقاً لتتجنب مقارنات غير ضرورية — O(n+m)."},{question:"ما أهمية ترميز UTF-8؟",options:["سرعة القراءة","دعم جميع لغات العالم في ترميز واحد","تقليل حجم الملف","منع الأخطاء"],correct:1,explanation:"UTF-8 يدعم كل أحرف يونيكود بما فيها العربية والصينية والإيموجي."},{question:"ما هو Tokenization في معالجة النصوص؟",options:["تشفير النص","تقطيع النص لوحدات ذات معنى (كلمات، رموز)","ضغط النص","طباعة النص"],correct:1,explanation:"Tokenization يقسم النص لرموز (tokens) كخطوة أولى في التحليل."}]},{sectionId:"l4_data_processing",questions:[{question:"ما هو ETL في معالجة البيانات؟",options:["لغة برمجة","Extract-Transform-Load: استخلاص وتحويل وتحميل","أداة اختبار","نمط تصميم"],correct:1,explanation:"ETL عملية استخلاص البيانات من المصدر وتحويلها وتحميلها للهدف."},{question:"ما أهمية التصفية (Filtering) في معالجة البيانات؟",options:["تزيد حجم البيانات","تختار البيانات المطلوبة فقط وتستبعد غير المرغوبة","تحذف جميع البيانات","تنسخ البيانات"],correct:1,explanation:"التصفية تقلل حجم البيانات للعمل فقط مع ما هو مطلوب."},{question:"ما هو التجميع (Aggregation)؟",options:["جمع ملفات","حساب إحصائيات مثل المجموع والمتوسط من مجموعة بيانات","حذف بيانات","ترتيب بيانات"],correct:1,explanation:"التجميع يلخّص بيانات كثيرة في إحصائيات (مجموع، عدد، متوسط)."},{question:"لماذا نستخدم المعالجة الدفعية (Batch Processing)؟",options:["لتبسيط الكود","لمعالجة البيانات الضخمة التي لا تتسع في الذاكرة مرة واحدة","لتسريع الطباعة","لمنع الأخطاء"],correct:1,explanation:"الدفعات تقسم البيانات الكبيرة لأجزاء صغيرة قابلة للمعالجة."},{question:"ما أهمية التحقق من جودة البيانات (Data Validation)؟",options:["شكلية فقط","منع إدخال بيانات خاطئة تؤثر على نتائج التحليل","تسريع المعالجة","تقليل حجم الملف"],correct:1,explanation:"Garbage In, Garbage Out — بيانات خاطئة تعطي نتائج خاطئة."}]},{sectionId:"l4_errors_reliability",questions:[{question:"ما هي معالجة الأخطاء الدفاعية (Defensive Programming)؟",options:["تجاهل الأخطاء","فحص المدخلات والافتراضات قبل تنفيذ العملية","حذف كود الأخطاء","استخدام try/catch فقط"],correct:1,explanation:"البرمجة الدفاعية تتحقق من صحة كل مدخل وافتراض مسبقاً."},{question:"ما هو نمط Circuit Breaker؟",options:["قطع الكهرباء","إيقاف الطلبات مؤقتاً عند تكرار الفشل ثم إعادة المحاولة","حذف السجلات","تسريع الاتصال"],correct:1,explanation:"Circuit Breaker يمنع تكرار الطلبات لخدمة معطلة ويعيد المحاولة لاحقاً."},{question:"ما أهمية إعادة المحاولة (Retry) مع تأخير متزايد؟",options:["لإبطاء البرنامج","للتعامل مع أخطاء مؤقتة دون إغراق الخدمة","لتوفير الذاكرة","لمنع الاختراق"],correct:1,explanation:"Exponential Backoff يزيد وقت الانتظار بين المحاولات لتجنب الضغط."},{question:"ما هو التسجيل المهيكل (Structured Logging)؟",options:["كتابة نصوص عشوائية","تسجيل أحداث بتنسيق منظم (JSON) مع بيانات وصفية","حذف السجلات القديمة","طباعة الأخطاء فقط"],correct:1,explanation:"التسجيل المهيكل يسهّل البحث والتصفية والتحليل الآلي للسجلات."},{question:"ما هو الانغلاق الجيد (Graceful Degradation)؟",options:["إيقاف البرنامج فوراً","تقديم خدمة جزئية عند فشل بعض المكونات بدلاً من التوقف الكامل","تخفيض الأداء","حذف البيانات"],correct:1,explanation:"Graceful Degradation يحافظ على عمل البرنامج ولو بقدرات محدودة."}]},{sectionId:"l4_applied_projects",questions:[{question:"ما أهمية التخطيط قبل كتابة الكود؟",options:["غير مهم","يوفر الوقت ويقلل إعادة العمل ويوضح المتطلبات","يُبطئ العمل","مطلوب فقط للمشاريع الكبيرة"],correct:1,explanation:"التخطيط يحدد المتطلبات والهيكل قبل البدء مما يوفر الوقت لاحقاً."},{question:"ما هو اختبار التكامل (Integration Testing)؟",options:["اختبار وحدة واحدة","اختبار عمل عدة وحدات معاً","اختبار الأداء","اختبار الأمان"],correct:1,explanation:"اختبار التكامل يتحقق من عمل المكونات المختلفة معاً بشكل صحيح."},{question:"ما أهمية التوثيق في المشاريع؟",options:["غير ضروري","يسهّل الصيانة ونقل المعرفة للمطورين الجدد","يزيد حجم الملف","يُبطئ البرنامج"],correct:1,explanation:"التوثيق يحفظ المعرفة ويسهّل على الآخرين فهم الكود وصيانته."},{question:"ما هو مبدأ KISS؟",options:["نمط تصميم","Keep It Simple Stupid — أبقِه بسيطاً","خوارزمية ترتيب","أداة اختبار"],correct:1,explanation:"KISS يعني تبسيط الحلول وتجنب التعقيد غير الضروري."},{question:"ما الفرق بين MVP و المنتج الكامل؟",options:["لا فرق","MVP يحتوي الحد الأدنى من الميزات للإطلاق السريع","MVP أغلى","المنتج الكامل أسرع في البناء"],correct:1,explanation:"MVP (الحد الأدنى القابل للتطبيق) يركز على الميزات الأساسية فقط."}]},{sectionId:"l4_advanced_challenges",questions:[{question:"ما هي تقنية Backtracking؟",options:["الرجوع في الكود","استكشاف كل الاحتمالات والتراجع عند عدم الحل","حذف التعديلات","نسخ الملفات"],correct:1,explanation:"Backtracking تجرب احتمالاً وتتراجع إذا لم يؤدِ لحل."},{question:"ما تعقيد خوارزمية Dijkstra لأقصر مسار؟",options:["O(n)","O(n²)","O(V² + E) أو O((V+E) log V)","O(log n)"],correct:2,explanation:"Dijkstra مع Heap يعمل بتعقيد O((V+E) log V)."},{question:"ما هو مبدأ Amortized Analysis؟",options:["تحليل كل عملية منفردة","حساب متوسط تكلفة العمليات على المدى الطويل","تحليل الأخطاء","تحليل الذاكرة"],correct:1,explanation:"التحليل المطفأ يحسب متوسط تكلفة سلسلة عمليات بدلاً من الأسوأ لكل عملية."},{question:"متى نستخدم البحث في العمق (DFS) مقابل البحث في العرض (BFS)؟",options:["لا فرق بينهما","DFS لاستكشاف كامل المسار، BFS لأقصر مسار","DFS أسرع دائماً","BFS للأشجار فقط"],correct:1,explanation:"DFS يستكشف بعمق أولاً، BFS يجد أقصر مسار في الرسوم غير الموزونة."},{question:"ما هو Bloom Filter؟",options:["مرشح صور","بنية بيانات احتمالية للتحقق السريع من العضوية","خوارزمية ترتيب","أداة تصحيح"],correct:1,explanation:'Bloom Filter يحدد بسرعة إن كان العنصر "ربما موجود" أو "حتماً غير موجود".'}]}],eu=[{sectionId:"l5_functional_advanced",questions:[{question:"ما هو الكاري (Currying)؟",options:["تسريع الدوال","تحويل دالة متعددة المعاملات لسلسلة دوال أحادية","دمج دوال","نوع من الأخطاء"],correct:1,explanation:"Currying يحوّل f(a, b, c) إلى f(a)(b)(c) للتركيب المرن."},{question:"ما هو Closure (النطاق المغلق)؟",options:["إغلاق الملف","دالة تحتفظ بمتغيرات نطاقها الخارجي","إنهاء البرنامج","حذف المتغيرات"],correct:1,explanation:"Closure يسمح للدالة بالوصول لمتغيرات النطاق الذي أُنشئت فيه."},{question:"ما الفرق بين map وfilter وreduce؟",options:["لا فرق","map تحوّل، filter تختار، reduce تختزل لقيمة واحدة","كلها تنشئ مصفوفات","كلها تحذف عناصر"],correct:1,explanation:"map: تحويل كل عنصر. filter: اختيار عناصر. reduce: تجميع لقيمة واحدة."},{question:"ما هي البيانات غير القابلة للتغيير (Immutability)؟",options:["بيانات مشفرة","بيانات لا يمكن تعديلها بعد إنشائها — إنما تُنتج نسخة جديدة","بيانات ثابتة القيمة","بيانات محذوفة"],correct:1,explanation:"Immutability: بدلاً من تعديل الأصل، أنشئ نسخة جديدة بالتعديلات."},{question:"ما هو التذكّر (Memoization)؟",options:["حفظ الملفات","تخزين نتائج الدوال لتجنب إعادة حسابها","حفظ الأخطاء","نسخ البيانات"],correct:1,explanation:"Memoization يحفظ نتيجة الدالة لنفس المدخلات — يحوّل O(2^n) لـ O(n)."}]},{sectionId:"l5_data_structures",questions:[{question:"ما تعقيد الإدراج في القائمة المترابطة؟",options:["O(n)","O(log n)","O(1)","O(n²)"],correct:2,explanation:"الإدراج في بداية القائمة المترابطة O(1) — مجرد تغيير مؤشر."},{question:"ما هو طابور الأولوية (Priority Queue)؟",options:["طابور عادي","طابور حيث العنصر ذو الأولوية العليا يخرج أولاً","مصفوفة مرتبة","مكدّس"],correct:1,explanation:"Priority Queue يخرج العنصر الأهم أولاً بغض النظر عن وقت الإضافة."},{question:"ما الفرق بين القائمة المترابطة الأحادية والثنائية؟",options:["لا فرق","الأحادية تشير للتالي فقط، والثنائية للتالي والسابق","الثنائية أبطأ","الأحادية تدعم الحذف فقط"],correct:1,explanation:"الثنائية تتيح التنقل في الاتجاهين مما يسهّل الحذف والتنقل العكسي."},{question:"ما هو Hash Table وما مشكلة التصادم فيه؟",options:["جدول عادي بدون مشاكل","جدول تجزئة حيث قد تتشارك مفاتيح مختلفة نفس الفهرس","مصفوفة متداخلة","قاعدة بيانات"],correct:1,explanation:"التصادم يحدث عندما تُنتج دالة التجزئة نفس الفهرس لمفاتيح مختلفة."},{question:"متى نستخدم الشجرة بدلاً من المصفوفة؟",options:["دائماً","عند الحاجة لبحث وإدراج وحذف سريع O(log n)","عند الحاجة لذاكرة أقل","عند العمل مع نصوص فقط"],correct:1,explanation:"الأشجار المتوازنة (BST) توفر O(log n) للبحث والإدراج والحذف."}]},{sectionId:"l5_algorithms",questions:[{question:"ما الفرق بين O(n log n) و O(n²) لترتيب مليون عنصر؟",options:["لا فرق ملحوظ","O(n log n) أسرع بآلاف المرات","O(n²) أسرع","يعتمد على المعالج فقط"],correct:1,explanation:"1M × log(1M) ≈ 20M عملية مقابل 1M² = 1T عملية — فرق هائل!"},{question:"ما هي تقنية المؤشرين (Two Pointers)؟",options:["استخدام متغيرين","مؤشران يتحركان في المصفوفة لتقليل التعقيد من O(n²) لـ O(n)","مصفوفتان","نوع من الأشجار"],correct:1,explanation:"Two Pointers يستبدل حلقتين متداخلتين بمؤشرين ذكيين."},{question:"ما هي النافذة المنزلقة (Sliding Window)؟",options:["واجهة مستخدم","تقنية لمعالجة نوافذ فرعية متتالية بكفاءة","نوع من الحلقات","بنية بيانات"],correct:1,explanation:"بدلاً من إعادة حساب كل نافذة، أضف العنصر الجديد وأزل القديم."},{question:"متى نستخدم Merge Sort بدلاً من Quick Sort؟",options:["دائماً","عندما نحتاج ترتيب مستقر وأداء O(n log n) مضمون","عندما المصفوفة صغيرة","عندما المصفوفة مرتبة"],correct:1,explanation:"Merge Sort مستقر وO(n log n) مضمون، بينما Quick Sort قد يكون O(n²)."},{question:"ما هو Big O Notation؟",options:["حجم البرنامج","تعبير رياضي يصف نمو وقت التنفيذ مع حجم المدخلات","عدد الأسطر","نوع من التعليقات"],correct:1,explanation:"Big O يصف الحد الأعلى لتعقيد الخوارزمية مع نمو المدخلات."}]},{sectionId:"l5_libraries_modules",questions:[{question:"ما هو مبدأ المسؤولية الواحدة (SRP)؟",options:["كل ملف يحوي صنفاً واحداً","كل وحدة مسؤولة عن شيء واحد فقط","كل متغير يُستخدم مرة","كل دالة سطر واحد"],correct:1,explanation:"SRP: كل وحدة/صنف يفعل شيئاً واحداً ويفعله جيداً."},{question:"ما هو Semantic Versioning (SemVer)؟",options:["ترقيم عشوائي","MAJOR.MINOR.PATCH — كسر/إضافة/إصلاح","ترقيم بالتاريخ","ترقيم أبجدي"],correct:1,explanation:"MAJOR: تغييرات غير متوافقة. MINOR: ميزات جديدة. PATCH: إصلاحات."},{question:"ما الفرق بين Loose Coupling و Tight Coupling؟",options:["لا فرق","Loose: الوحدات مستقلة قليلة الاعتماد. Tight: متشابكة.","Tight أفضل دائماً","Loose أبطأ"],correct:1,explanation:"Loose Coupling يسهّل التعديل والاختبار لأن الوحدات مستقلة."},{question:"ما هو Method Chaining (سلسلة الاستدعاءات)؟",options:["استدعاء دالة واحدة","كل دالة تُرجع this لتتيح استدعاء دالة أخرى مباشرة","تكرار الدالة","ربط ملفات"],correct:1,explanation:"كائن.دالة1().دالة2().دالة3() — كل دالة تُرجع الكائن."},{question:"ما أهمية ملف README في المكتبة؟",options:["ليس مهماً","يشرح كيفية التثبيت والاستخدام والأمثلة","يحتوي كود البرنامج","يخزن إعدادات الحاسوب"],correct:1,explanation:"README أول ما يراه المستخدم — يجب أن يشرح الغرض والاستخدام بوضوح."}]},{sectionId:"l5_cli_applications",questions:[{question:"ما هو CLI (واجهة سطر الأوامر)؟",options:["واجهة رسومية","تفاعل مع البرنامج عبر أوامر نصية","لغة برمجة","نظام تشغيل"],correct:1,explanation:"CLI يتيح التفاعل مع البرنامج بكتابة أوامر نصية في الطرفية."},{question:"ما هي الوسائط (Arguments) في أوامر CLI؟",options:["أخطاء","قيم تُمرر للبرنامج عند تشغيله","ملفات إعدادات","متغيرات بيئة"],correct:1,explanation:"مثل: برنامج --output file.txt حيث output و file.txt وسائط."},{question:"ما هو نمط REPL؟",options:["لغة برمجة","Read-Eval-Print-Loop: دورة قراءة-تنفيذ-طباعة","أداة اختبار","نظام ملفات"],correct:1,explanation:"REPL بيئة تفاعلية: اقرأ مدخل → نفّذه → اطبع النتيجة → كرر."},{question:"ما أهمية رسالة --help في أدوات CLI؟",options:["ليست مهمة","تشرح كيفية استخدام الأداة وخياراتها المتاحة","تفتح موقع الويب","تحذف البرنامج"],correct:1,explanation:"--help يوفر توثيقاً ذاتياً يشرح الأوامر والخيارات المتاحة."},{question:"ما هو Exit Code (رمز الخروج)؟",options:["كلمة سر","رقم يدل على نجاح (0) أو فشل (غير 0) تنفيذ البرنامج","رقم عشوائي","عدد الأسطر"],correct:1,explanation:"Exit Code 0 = نجاح. أي رقم آخر = فشل مع نوع الخطأ."}]},{sectionId:"l5_data_processing",questions:[{question:"ما هو CSV وكيف يختلف عن JSON؟",options:["لا فرق","CSV جدولي بسيط بفواصل، JSON هيكلي بأزواج مفتاح/قيمة","CSV أحدث","JSON للأرقام فقط"],correct:1,explanation:"CSV: بيانات جدولية بسيطة. JSON: بيانات هيكلية متداخلة."},{question:"ما هو خط أنابيب التحويل (Transformation Pipeline)؟",options:["أنبوب مادي","سلسلة عمليات تحويل متتالية تمرر البيانات بينها","نوع ملف","قاعدة بيانات"],correct:1,explanation:"Pipeline: البيانات تدخل وتمر بمحطات تحويل متتالية حتى النتيجة."},{question:"ما أهمية GROUP BY في تحليل البيانات؟",options:["ترتيب البيانات","تجميع البيانات حسب فئة وحساب إحصائيات لكل مجموعة","حذف التكرارات","تصفية البيانات"],correct:1,explanation:"GROUP BY يجمع البيانات بمعايير ويحسب إحصائيات لكل مجموعة."},{question:'ما معنى "Garbage In, Garbage Out"؟',options:["حذف البيانات القديمة","بيانات خاطئة تعطي نتائج خاطئة — التحقق ضروري","تنظيف الذاكرة","إعادة تشغيل البرنامج"],correct:1,explanation:"إذا كانت البيانات المدخلة خاطئة، ستكون النتائج خاطئة أيضاً."},{question:"ما الفرق بين المعالجة الدفعية (Batch) والمباشرة (Stream)؟",options:["لا فرق","Batch تعالج مجموعة كاملة، Stream تعالج عنصراً بعنصر فور وصوله","Batch أسرع دائماً","Stream للملفات فقط"],correct:1,explanation:"Batch: معالجة دفعة كاملة. Stream: معالجة فورية مستمرة."}]},{sectionId:"l5_testing_quality",questions:[{question:"ما هو نمط AAA في الاختبارات؟",options:["ثلاث تأكيدات","Arrange-Act-Assert: إعداد-تنفيذ-تأكيد","ثلاث محاولات","اختبار ثلاثي"],correct:1,explanation:"AAA: رتّب البيانات (Arrange)، نفّذ العملية (Act)، تأكد من النتيجة (Assert)."},{question:"ما هو TDD (التطوير المقاد بالاختبارات)؟",options:["كتابة الكود ثم الاختبارات","كتابة الاختبار أولاً ثم الكود","اختبار يدوي فقط","اختبار بعد النشر"],correct:1,explanation:"TDD: أحمر (اختبار فاشل) → أخضر (كود ناجح) → إعادة هيكلة."},{question:"ما هو Mocking في الاختبارات؟",options:["السخرية من الكود","استبدال تبعيات خارجية بكائنات وهمية للاختبار","حذف الاختبارات","تسريع الاختبارات"],correct:1,explanation:"Mocking يعزل الوحدة عن تبعياتها (API, DB) باستخدام بدائل مُتحكم بها."},{question:"ما هو هرم الاختبارات (Testing Pyramid)؟",options:["شكل جمالي","وحدوي كثير (القاعدة) > تكامل معقول > E2E قليل (القمة)","اختبار من القمة للقاعدة","ترتيب الملفات"],correct:1,explanation:"القاعدة: اختبارات وحدوية سريعة كثيرة. القمة: E2E بطيئة قليلة."},{question:"ما أهمية Code Coverage (تغطية الكود)؟",options:["قياس سرعة الكود","قياس نسبة الكود الذي تمر عليه الاختبارات","عدد الملفات","حجم البرنامج"],correct:1,explanation:"تغطية 80%+ تعطي ثقة بأن معظم الكود مختبر ويعمل صحيحاً."}]},{sectionId:"l5_complete_projects",questions:[{question:"ما أول خطوة في بناء مشروع متكامل؟",options:["كتابة الكود فوراً","تحليل المتطلبات وتصميم الهيكل","اختيار الألوان","نشر المشروع"],correct:1,explanation:"التحليل والتصميم قبل الكتابة يوفر الوقت ويقلل الأخطاء."},{question:"ما هو نمط MVC؟",options:["لغة برمجة","Model-View-Controller: فصل البيانات والعرض والتحكم","أداة اختبار","نوع قاعدة بيانات"],correct:1,explanation:"MVC يفصل المنطق (Model) والعرض (View) والتحكم (Controller)."},{question:"ما أهمية معالجة الأخطاء في المشاريع الحقيقية؟",options:["غير مهمة","حاسمة لمنع التوقف المفاجئ وحماية البيانات","شكلية فقط","لتسريع البرنامج"],correct:1,explanation:"المشاريع الحقيقية تواجه أخطاء غير متوقعة — معالجتها ضرورية."},{question:"ما هو Refactoring (إعادة الهيكلة)؟",options:["إعادة كتابة المشروع من الصفر","تحسين هيكل الكود بدون تغيير سلوكه","إضافة ميزات جديدة","حذف التعليقات"],correct:1,explanation:"Refactoring: نفس السلوك، كود أنظف — يسهّل الصيانة والتوسع."},{question:"ما الذي يميّز المبرمج المحترف عن المبتدئ؟",options:["سرعة الكتابة","القدرة على تصميم أنظمة قابلة للتوسع والصيانة","حفظ كل الدوال","عدد اللغات المعروفة"],correct:1,explanation:"المحترف يصمم أنظمة مرنة، يكتب كوداً نظيفاً، ويفكر بالمستقبل."}]}],tu={1:K_,2:Y_,3:X_,4:Z_,5:eu};function vt(e,n){const s=tu[e];if(s)return s.find(a=>a.sectionId===n)}const nu={class:"learning-path",dir:"rtl"},su={class:"lp-header"},ou={class:"lp-particles","aria-hidden":"true"},iu={class:"lp-header-content"},au={class:"lp-subtitle"},lu={class:"lp-global-progress"},cu={class:"gp-row"},ru={class:"gp-pct"},du={class:"gp-bar"},_u={class:"gp-detail"},uu={class:"resume-text"},pu={key:1,class:"notes-review-trigger"},vu={key:0},hu=w({__name:"LearningPath",setup(e){const{completedCount:n,isCompleted:s,markCompleted:a,getLevelProgress:o,getSectionProgress:c,setLastLesson:r,getLastLevel:d,getLastLesson:_}=Nc(),p=Ac,m=C(1),h=C(null),S=C(null),f=O(()=>p.find(B=>B.id===m.value)),b=O(()=>{var B;return((B=f.value)==null?void 0:B.sections)||[]}),I=O(()=>{var B;return((B=f.value)==null?void 0:B.color)||"#10b981"}),M=O(()=>{if(!S.value)return[];const B=b.value.find(R=>R.id===S.value);return(B==null?void 0:B.lessons)||[]}),H=O(()=>h.value?M.value.findIndex(B=>B.id===h.value.id):0),V=O(()=>p.reduce((B,R)=>B+R.sections.reduce((Y,te)=>Y+te.lessons.length,0),0)),P=O(()=>p.flatMap(B=>B.sections.flatMap(R=>R.lessons.map(Y=>Y.id)))),L=O(()=>{if(P.value.length===0)return 0;const B=P.value.filter(R=>s(R)).length;return Math.round(B/P.value.length*100)}),T=O(()=>{const B=L.value;return B<25?"linear-gradient(90deg, #10b981, #34d399)":B<50?"linear-gradient(90deg, #10b981, #3b82f6)":B<75?"linear-gradient(90deg, #3b82f6, #8b5cf6)":"linear-gradient(90deg, #8b5cf6, #f59e0b)"}),k=O(()=>{const B=_();if(!B)return null;for(const R of p)for(const Y of R.sections){const te=Y.lessons.find(le=>le.id===B);if(te)return{lesson:te,sectionId:Y.id,levelId:R.id,title:te.title}}return null});function $(){const B=k.value;B&&(m.value=B.levelId,J(B.lesson,B.sectionId))}function D(B){const R=p.find(te=>te.id===B);if(!R)return 0;const Y=R.sections.flatMap(te=>te.lessons.map(le=>le.id));return o(B,Y)}function K(B){return c(B)}function G(B){m.value=B,h.value=null,S.value=null}function J(B,R){h.value=B,S.value=R,r(B.id,m.value)}function z(){h.value=null,S.value=null}function ie(){h.value&&a(h.value.id)}function pe(){const B=H.value;B<M.value.length-1&&(h.value=M.value[B+1],r(h.value.id,m.value))}function ve(){const B=H.value;B>0&&(h.value=M.value[B-1],r(h.value.id,m.value))}ee(()=>{m.value=d();const{loadWasm:B}=It();B()});const{getNotesCount:he,getBookmarksCount:fe}=lt(),ae=C(!1),be=O(()=>he()),ye=O(()=>fe());function me(B,R,Y){if(R&&(m.value=R),Y){const te=b.value.find(le=>le.id===Y);if(te){const le=te.lessons.find(Ie=>Ie.id===B);if(le){J(le,Y);return}}}for(const te of p)for(const le of te.sections){const Ie=le.lessons.find(Dt=>Dt.id===B);if(Ie){m.value=te.id,J(Ie,le.id);return}}}const $e=C(!1),ge=C(null);function ze(B){const R=vt(m.value,B);return!!R&&R.questions.length>0}function Ue(B,R){const Y=vt(m.value,B);Y&&(ge.value={title:`اختبار: ${R}`,questions:Y.questions},$e.value=!0)}function Se(B,R){$e.value=!1,ge.value=null}return(B,R)=>(i(),l("div",nu,[t("div",su,[t("div",ou,[(i(),l(F,null,j(20,Y=>t("span",{key:Y,class:N(["dot","dot-"+Y])},null,2)),64))]),t("div",iu,[R[7]||(R[7]=t("div",{class:"lp-badge"},"مسار تفاعلي",-1)),R[8]||(R[8]=t("h1",{class:"lp-title"},[t("span",{class:"title-deco"},"❰"),Q(" المسار التعليمي للغة ص "),t("span",{class:"title-deco"},"❱")],-1)),t("p",au,v(V.value)+" درس تفاعلي — من الصفر إلى الاحتراف",1),t("div",lu,[t("div",cu,[R[3]||(R[3]=t("span",{class:"gp-label"},"التقدم الكلي",-1)),t("span",ru,v(L.value)+"%",1)]),t("div",du,[t("div",{class:"gp-fill",style:W({width:L.value+"%",background:T.value})},[...R[4]||(R[4]=[t("div",{class:"gp-shimmer"},null,-1)])],4)]),t("span",_u,v(u(n))+" من "+v(V.value)+" درس مكتمل",1)]),k.value&&!h.value?(i(),l("button",{key:0,class:"lp-resume",onClick:$},[R[5]||(R[5]=t("span",{class:"resume-icon"},[t("svg",{width:"16",height:"16",viewBox:"0 0 24 24",fill:"currentColor"},[t("path",{d:"M8 5v14l11-7z"})])],-1)),t("span",uu,"متابعة: "+v(k.value.title),1),R[6]||(R[6]=t("span",{class:"resume-arrow"},"←",-1))])):g("",!0)])]),E(Tt,{levels:u(p),"active-level":m.value,"get-progress":D,onSelect:G},null,8,["levels","active-level"]),E(se,{name:"view-fade",mode:"out-in"},{default:y(()=>[h.value?(i(),A(Et,{key:"wizard",lesson:h.value,"lesson-index":H.value,"total-lessons":M.value.length,"is-completed":u(s)(h.value.id),"level-color":I.value,"section-id":S.value||void 0,"level-id":m.value,onClose:z,onComplete:ie,onNext:pe,onPrev:ve},null,8,["lesson","lesson-index","total-lessons","is-completed","level-color","section-id","level-id"])):(i(),A(Ot,{key:"sections",sections:b.value,"level-color":I.value,"is-completed":u(s),"get-section-progress":K,"has-quiz":ze,onOpenLesson:J,onStartQuiz:Ue},null,8,["sections","level-color","is-completed"]))]),_:1}),h.value?g("",!0):(i(),l(F,{key:0},[E(qt,{"completed-count":u(n),"total-lessons":V.value},null,8,["completed-count","total-lessons"]),E(Mt)],64)),!h.value&&be.value>0?(i(),l("div",pu,[t("button",{class:"notes-review-btn",onClick:R[0]||(R[0]=Y=>ae.value=!0)},[R[9]||(R[9]=t("span",null,"📓",-1)),t("span",null,"ملاحظاتي ("+v(be.value)+")",1),ye.value>0?(i(),l("span",vu,"⭐ "+v(ye.value),1)):g("",!0)])])):g("",!0),$e.value&&ge.value?(i(),A(At,{key:2,title:ge.value.title,questions:ge.value.questions,"level-color":I.value,onClose:R[1]||(R[1]=Y=>$e.value=!1),onComplete:Se},null,8,["title","questions","level-color"])):g("",!0),ae.value?(i(),A(W_,{key:3,"level-color":I.value,onClose:R[2]||(R[2]=Y=>ae.value=!1),onGoToLesson:me},null,8,["level-color"])):g("",!0)]))}}),fu=q(hu,[["__scopeId","data-v-3b40753b"]]),gu={extends:rt,Layout(){return Zt(rt.Layout,null,{"doc-before":()=>null})},enhanceApp({app:e,router:n}){e.component("HomeHero",Da),e.component("DownloadCard",Qa),e.component("CodePlayground",vl),e.component("FeatureGrid",kl),e.component("SadVersion",Sl),e.component("InteractiveTutorial",Wl),e.component("LearningPath",fu),e.component("LevelTabs",Tt),e.component("SectionList",Ot),e.component("LessonWizard",Et),e.component("SadCodeRunner",Ye),e.component("LearningStats",qt),e.component("AchievementBadges",Mt),e.component("QuizModal",At),typeof window<"u"&&(n.onAfterRouteChanged=()=>{document.documentElement.dir="rtl",document.documentElement.lang="ar"})}};export{gu as R,U as u};
